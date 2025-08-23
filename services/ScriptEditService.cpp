//
// Created by Orill on 4/16/2025.
//

#include "ScriptEditService.h"
#include "InstallationVerifier.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>

namespace DosboxStagingReplacer {

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
                    // Replace all back relative path to absolute path
                    replaceAll(line, "..", basePath.string());
                    // Replace all current relative path to absolute path
                    replaceAll(line, ".", basePath.string());
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

        const auto pos = params.find("mount");
        if (pos == std::string::npos) {
            // If "mount" isn't found, return the input unchanged (conservative fallback).
            return params;
        }

        const std::string mountParams = params.substr(pos + 5); // characters after "mount"

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

        return "mount " + targetDrive + " " + targetPath;
    }


} // DosboxStagingReplacer