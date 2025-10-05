//

#include "ScriptEditService.h"
#include "InstallationVerifier.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

namespace DosboxStagingReplacer {

    namespace {
        std::string toLowerCopy(const std::string &value) {
            std::string lower = value;
            std::ranges::transform(lower, lower.begin(), [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            });
            return lower;
        }

        std::vector<std::string> splitDosboxArgs(const std::string &line) {
            std::vector<std::string> tokens;
            std::string current;
            bool inQuotes = false;
            char quoteChar = '\0';
            for (char c : line) {
                if (inQuotes) {
                    current.push_back(c);
                    if (c == quoteChar) {
                        inQuotes = false;
                    }
                    continue;
                }

                if (c == '"' || c == '\'') {
                    inQuotes = true;
                    quoteChar = c;
                    current.push_back(c);
                }
                else if (std::isspace(static_cast<unsigned char>(c))) {
                    if (!current.empty()) {
                        tokens.push_back(current);
                        current.clear();
                    }
                }
                else {
                    current.push_back(c);
                }
            }

            if (!current.empty()) {
                tokens.push_back(current);
            }

            return tokens;
        }

        std::string joinDosboxArgs(const std::vector<std::string> &tokens) {
            std::ostringstream out;
            for (std::size_t i = 0; i < tokens.size(); ++i) {
                if (i != 0) {
                    out << ' ';
                }
                out << tokens[i];
            }
            return out.str();
        }

        bool isQuotedToken(const std::string &token, char *quoteCharOut = nullptr) {
            if (token.size() >= 2) {
                const char first = token.front();
                const char last = token.back();
                if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
                    if (quoteCharOut != nullptr) {
                        *quoteCharOut = first;
                    }
                    return true;
                }
            }
            return false;
        }

        std::string stripQuotationMarks(const std::string &token) {
            if (token.size() >= 2) {
                char quoteChar = '\0';
                if (isQuotedToken(token, &quoteChar)) {
                    return token.substr(1, token.size() - 2);
                }
            }
            return token;
        }

        std::size_t findMountCommandIndex(const std::vector<std::string> &tokens) {
            for (std::size_t i = 0; i < tokens.size(); ++i) {
                auto lowerToken = toLowerCopy(tokens[i]);
                if (!lowerToken.empty() && lowerToken.front() == '@') {
                    lowerToken = lowerToken.substr(1);
                }
                if (lowerToken == "mount" || lowerToken == "imgmount") {
                    return i;
                }
            }
            return tokens.size();
        }

        bool tokenIsOption(const std::string &token) {
            return !token.empty() && token.front() == '-';
        }

        bool shouldTreatAsPathToken(const std::vector<std::string> &tokens,
                                    std::size_t commandIdx,
                                    std::size_t idx) {
            if (idx <= commandIdx) {
                return false;
            }
            if (idx == commandIdx + 1) {
                // Typically the drive or disk identifier.
                return false;
            }
            if (tokenIsOption(tokens[idx])) {
                return false;
            }
            if (idx > 0 && tokenIsOption(tokens[idx - 1])) {
                // Option value (e.g. -t iso, -fs iso).
                return false;
            }
            return true;
        }

        std::string restoreLeadingIndent(const std::string &original, const std::string &processed) {
            const std::size_t firstNonSpace = original.find_first_not_of(" \t");
            if (firstNonSpace == std::string::npos) {
                return processed;
            }
            const std::string prefix = original.substr(0, firstNonSpace);
            return prefix + processed;
        }

        std::filesystem::path clampRelativeToBase(const std::filesystem::path &base,
                                                  const std::filesystem::path &relative) {
            std::vector<std::filesystem::path> segments;
            for (const auto &part : relative) {
                const std::string segment = part.string();
                if (segment == "." || segment.empty()) {
                    continue;
                }
                if (segment == "..") {
                    if (!segments.empty()) {
                        segments.pop_back();
                    }
                    continue;
                }
                segments.push_back(part);
            }

            std::filesystem::path resolved = base;
            for (const auto &segment : segments) {
                resolved /= segment;
            }
            return resolved;
        }

        std::string resolveMountLinePaths(const std::string &line,
                                          const std::filesystem::path &basePath) {
            const std::size_t firstNonSpace = line.find_first_not_of(" \t");
            const std::string trimmed = (firstNonSpace == std::string::npos)
                                        ? std::string{}
                                        : line.substr(firstNonSpace);

            auto tokens = splitDosboxArgs(trimmed);
            if (tokens.empty()) {
                return line;
            }

            const std::size_t commandIdx = findMountCommandIndex(tokens);
            if (commandIdx == tokens.size()) {
                return line;
            }

            bool updated = false;
            for (std::size_t idx = commandIdx + 1; idx < tokens.size(); ++idx) {
                if (!shouldTreatAsPathToken(tokens, commandIdx, idx)) {
                    continue;
                }

                char quoteChar = '\0';
                const bool wasQuoted = isQuotedToken(tokens[idx], &quoteChar);
                const std::string body = stripQuotationMarks(tokens[idx]);
                if (body.empty()) {
                    continue;
                }

                std::filesystem::path candidate(body);
                if (!candidate.empty() && candidate.is_relative()) {
                    const std::filesystem::path resolved = clampRelativeToBase(basePath, candidate);
                    const std::string resolvedStr = resolved.string();
                    const bool needsQuotes = wasQuoted || resolvedStr.find(' ') != std::string::npos;
                    if (needsQuotes) {
                        const char wrapChar = wasQuoted ? quoteChar : '"';
                        tokens[idx] = std::string(1, wrapChar) + resolvedStr + wrapChar;
                    } else {
                        tokens[idx] = resolvedStr;
                    }
                    updated = true;
                }
            }

            if (!updated) {
                return line;
            }

            const std::string rebuilt = joinDosboxArgs(tokens);
            return restoreLeadingIndent(line, rebuilt);
        }
    } // namespace

    void ScriptEditService::replaceAll(std::string& str, const std::string& from, const std::string& to) {
        if (from.empty()) return; // avoid infinite loop

        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // advance past the replacement
        }
    }

    bool ScriptEditService::isConfigFileDosboxAutoExec(const std::filesystem::path &filePath) {
        // We open the file and check if it contains the string "[autoexec] (case-insensitive)"
        if (std::fstream file(filePath); file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                auto lowerLine = line;
                std::ranges::transform(lowerLine, lowerLine.begin(), tolower);
                if (lowerLine.find("[autoexec]") != std::string::npos) {
                    return true;
                }
            }
        }
        return false;
    }

    bool ScriptEditService::isConfigFileDosboxConfig(const std::filesystem::path &filePath) {
        // We open the file and check if it contains the string "[autoexec] (case-insensitive)"
        if (std::fstream file(filePath); file.is_open()) {
            std::string line;
            bool foundSDLFlag = false;
            bool foundDosboxFlag = false;
            while (std::getline(file, line)) {
                auto lowerLine = line;
                std::ranges::transform(lowerLine, lowerLine.begin(), tolower);
                if (lowerLine.find("[sdl]") != std::string::npos) {
                    foundSDLFlag = true;
                }
                else if (lowerLine.find("[dosbox]") != std::string::npos) {
                    foundDosboxFlag = true;
                }
            }
            return foundSDLFlag && foundDosboxFlag;
        }
        return false;
    }

    std::string ScriptEditService::transformConfArg(const std::string &arg, const std::filesystem::path &basePath) {
        std::size_t lastDot = std::string::npos;
        bool hasSeparator = false;

        for (std::size_t i = 0; i < arg.size(); ++i) {
            const char c = arg[i];
            if (c == '"') { continue; }
            if (c == '.') { lastDot = i; continue; }
            break;
        }

        std::string out = arg;
        if (lastDot != std::string::npos) {
            std::string tail = (lastDot + 1 < arg.size()) ? arg.substr(lastDot + 1) : std::string{};
            std::string tail2 = (lastDot + 1 < arg.size()) ? arg.substr(lastDot + 1) : std::string{};

            if (tail.back() == '"') {
                tail = tail.substr(0, tail.size() - 1);
            }

            if (basePath.string().back() == '\\' && tail.front() == '\\') {
                tail = tail.substr(1);
                hasSeparator = true;
            }
            else if (basePath.string().back() == '\\' || tail.front() == '\\') {
                hasSeparator = true;
            }
            out = basePath.string() + (hasSeparator ? "" : "\\") + tail;
        } else {
            if (basePath.string().back() == '\\' || out.front() == '\\') {
                hasSeparator = true;
            }
            else if (basePath.string().back() == '\\' && out.front() == '\\') {
                out = out.substr(1);
                hasSeparator = true;
            }
            out = basePath.string() + (hasSeparator ? "" : "\\") + out;
        }

        if (out.empty() || out.front() != '"')
            out = "\"" + out + "\"";

        return out;
    }

    std::string ScriptEditService::resolveRelativePathsFromString(const std::string &cmd,
                                                                  const std::filesystem::path &basePath) {
        std::istringstream in(cmd);
        std::ostringstream out;
        std::string token;
        bool first = true;

        while (in >> token) {
            if (!first) out << ' ';
            first = false;

            if (token == "-conf") {
                out << token;
                if (std::string param; in >> param) {
                    out << ' ' << transformConfArg(param, basePath);
                }
            } else {
                out << token;
            }
        }
        return out.str();
    }

    void ScriptEditService::resolveRelativePathsForDosboxAutoExec(std::filesystem::path &filePath,
                                                                  const std::filesystem::path &basePath,
                                                                  const std::string &tmpExtension) {
        // We open two files, the original file and a tmp file
        std::fstream file(filePath);
        std::filesystem::path tmpFilePath = filePath;
        tmpFilePath += tmpExtension;

        // Double check first if the tmp file exists, if so we do something like .tmp2, tmp3, etc
        if (fileExists(tmpFilePath.string())) {
            int counter = 2;
            while (fileExists(tmpFilePath.string())) {
                tmpFilePath = filePath;
                tmpFilePath += tmpExtension + std::to_string(counter);
                counter++;
            }
        }
        std::fstream tmpFile(tmpFilePath, std::ios::out);

        // We slowly stream the file line by line and replace each line with any modifications
        if (file.is_open() && tmpFile.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Check if the line contains a path
                auto lowerLine = line;
                std::ranges::transform(lowerLine, lowerLine.begin(), tolower);
                // Check if the line contains the strings "imgmount" or "mount" (case-insensitive)
                if (lowerLine.find("mount") != std::string::npos || lowerLine.find("imgmount") != std::string::npos) {
                    line = sanitizeDosboxMountPath(line);
                    line = resolveMountLinePaths(line, basePath);
                }
                // Write the modified line to the tmp file
                tmpFile << line << std::endl;
            }
        }

        // Close the files
        file.close();
        tmpFile.close();
        // Remove the original file
        std::filesystem::remove(filePath);
        // Rename the tmp file to the original file
        std::filesystem::rename(tmpFilePath, filePath);
        // We also remove the tmp file
        std::filesystem::remove(tmpFilePath);
    }

    void ScriptEditService::disableFullScreenForDosboxConfig(std::filesystem::path &filePath,
                                                             const std::string &tmpExtension) {
        // We open two files, the original file and a tmp file
        std::fstream file(filePath);
        std::filesystem::path tmpFilePath = filePath;
        tmpFilePath += tmpExtension;

        // Double check first if the tmp file exists, if so we do something like .tmp2, tmp3, etc
        if (fileExists(tmpFilePath.string())) {
            int counter = 2;
            while (fileExists(tmpFilePath.string())) {
                tmpFilePath = filePath;
                tmpFilePath += tmpExtension + std::to_string(counter);
                counter++;
            }
        }
        std::fstream tmpFile(tmpFilePath, std::ios::out);

        // We slowly stream the file line by line and replace each line with any modifications
        if (file.is_open() && tmpFile.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Check if the line contains a path
                auto lowerLine = line;
                std::ranges::transform(lowerLine, lowerLine.begin(), tolower);
                if (lowerLine.find("fullscreen=true") != std::string::npos) {
                    replaceAll(line, "fullscreen=true", "fullscreen=false");
                }
                // Write the modified line to the tmp file
                tmpFile << line << std::endl;
            }
        }

        // Close the files
        file.close();
        tmpFile.close();
        // Remove the original file
        std::filesystem::remove(filePath);
        // Rename the tmp file to the original file
        std::filesystem::rename(tmpFilePath, filePath);
        // We also remove the tmp file
        std::filesystem::remove(tmpFilePath);
    }
    void ScriptEditService::disableOverwrittenMappingForDosboxConfig(std::filesystem::path &filePath,
                                                                     const std::string &tmpExtension) {
        // We open two files, the original file and a tmp file
        std::fstream file(filePath);
        std::filesystem::path tmpFilePath = filePath;
        tmpFilePath += tmpExtension;

        // Double check first if the tmp file exists, if so we do something like .tmp2, tmp3, etc
        if (fileExists(tmpFilePath.string())) {
            int counter = 2;
            while (fileExists(tmpFilePath.string())) {
                tmpFilePath = filePath;
                tmpFilePath += tmpExtension + std::to_string(counter);
                counter++;
            }
        }
        std::fstream tmpFile(tmpFilePath, std::ios::out);

        // We slowly stream the file line by line and replace each line with any modifications
        if (file.is_open() && tmpFile.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Check if the line contains a path
                auto lowerLine = line;
                std::ranges::transform(lowerLine, lowerLine.begin(), tolower);
                if (lowerLine.find("mapperfile=") == std::string::npos) {
                    tmpFile << line << std::endl;
                }
            }
        }

        // Close the files
        file.close();
        tmpFile.close();
        // Remove the original file
        std::filesystem::remove(filePath);
        // Rename the tmp file to the original file
        std::filesystem::rename(tmpFilePath, filePath);
        // We also remove the tmp file
        std::filesystem::remove(tmpFilePath);
    }
    void ScriptEditService::replaceDisplayToDefaultForDosboxConfig(std::filesystem::path &filePath,
                                                                   const std::string &tmpExtension) {
        // We open two files, the original file and a tmp file
        std::fstream file(filePath);
        std::filesystem::path tmpFilePath = filePath;
        tmpFilePath += tmpExtension;

        // Double check first if the tmp file exists, if so we do something like .tmp2, tmp3, etc
        if (fileExists(tmpFilePath.string())) {
            int counter = 2;
            while (fileExists(tmpFilePath.string())) {
                tmpFilePath = filePath;
                tmpFilePath += tmpExtension + std::to_string(counter);
                counter++;
            }
        }
        std::fstream tmpFile(tmpFilePath, std::ios::out);

        // We slowly stream the file line by line and replace each line with any modifications
        if (file.is_open() && tmpFile.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Check if the line contains a path
                auto lowerLine = line;
                std::ranges::transform(lowerLine, lowerLine.begin(), tolower);
                if (lowerLine.find("fullresolution=") != std::string::npos) {
                    // Find the value of fullresolution and if the value is not desktop we replace it
                    // what is the default
                    if (auto fullResolutionValue = line.substr(line.find('=') + 1); fullResolutionValue != "desktop") {
                        replaceAll(line, fullResolutionValue, "desktop");
                    }
                }
                else if (lowerLine.find("windowresolution=") != std::string::npos) {
                    // Find the value of windowresolution and if the value is not desktop we replace it
                    // what is the default
                    std::cerr << "windowresolution -> " << line << " " << filePath << std::endl;
                    if (auto windowResolutionValue = line.substr(line.find('=') + 1);
                        windowResolutionValue != "original") {
                        replaceAll(line, windowResolutionValue, "original");
                    }
                }
                // Write the modified line to the tmp file
                tmpFile << line << std::endl;
            }
        }

        // Close the files
        file.close();
        tmpFile.close();
        // Remove the original file
        std::filesystem::remove(filePath);
        // Rename the tmp file to the original file
        std::filesystem::rename(tmpFilePath, filePath);
        // We also remove the tmp file
        std::filesystem::remove(tmpFilePath);
    }

std::string ScriptEditService::sanitizeDosboxMountPath(const std::string &params) {
        // Replicates the Python logic provided, keeping behavior consistent.
        std::string targetDrive;
        std::string targetPath;
        std::string command = "imgmount";

        // Try to find imgmount first
        auto pos = params.find("imgmount");

        // If there is no match, we find mount
        if (pos == std::string::npos) {
            pos = params.find("mount");
            command = "mount";
            if (pos == std::string::npos) {
                return params;
            }
        }

        const std::string mountParams = params.substr(pos + command.length()); // characters after "mount"

        bool startParsing = false;
        std::size_t lastIndex = std::string::npos;

        // Parse targetDrive (the first non-space token after "mount")
        for (std::size_t idx = 0; idx < mountParams.size(); ++idx) {
            const char c = mountParams[idx];

            if (c != ' ') startParsing = true;

            if (startParsing) {
                if (c == ' ') {
                    lastIndex = idx;
                    break;
                } else {
                    targetDrive.push_back(c);
                }
            }
        }

        // Parse targetPath (the rest after the first separating space)
        if (lastIndex != std::string::npos) {
            const std::string pathParams = (lastIndex + 1 < mountParams.size())
                                           ? mountParams.substr(lastIndex + 1)
                                           : std::string{};

            startParsing = false;
            for (const char c : pathParams) {
                if (c != ' ') startParsing = true;
                if (startParsing) targetPath.push_back(c);
            }

            if (!targetPath.empty() && targetPath.front() != '"') {
                targetPath = "\"" + targetPath + "\"";
            }
        }

        return command + " " + targetDrive + " " + targetPath;
    }


} // DosboxStagingReplacer