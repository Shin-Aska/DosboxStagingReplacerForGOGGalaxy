#include "DosPathResolver.h"

#include <algorithm>
#include <cctype>
#include <ranges>
#include <sstream>

namespace DosboxStagingReplacer {

    std::string DosPathResolver::toLowerCopy(const std::string &value) {
        std::string lower = value;
        std::ranges::transform(lower, lower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return lower;
    }

    std::vector<std::string> DosPathResolver::splitDosboxArgs(const std::string &line) {
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
            } else if (std::isspace(static_cast<unsigned char>(c))) {
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

    std::string DosPathResolver::joinDosboxArgs(const std::vector<std::string> &tokens) {
        std::ostringstream out;
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            if (i != 0) {
                out << ' ';
            }
            out << tokens[i];
        }
        return out.str();
    }

    bool DosPathResolver::isQuotedToken(const std::string &token, char *quoteCharOut) {
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

    std::string DosPathResolver::stripQuotationMarks(const std::string &token) {
        if (token.size() >= 2) {
            char quoteChar = '\0';
            if (isQuotedToken(token, &quoteChar)) {
                return token.substr(1, token.size() - 2);
            }
        }
        return token;
    }

    std::size_t DosPathResolver::findMountCommandIndex(const std::vector<std::string> &tokens) {
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

    bool DosPathResolver::tokenIsOption(const std::string &token) {
        return !token.empty() && token.front() == '-';
    }

    bool DosPathResolver::shouldTreatAsPathToken(const std::vector<std::string> &tokens,
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

    std::string DosPathResolver::restoreLeadingIndent(const std::string &original, const std::string &processed) {
        const std::size_t firstNonSpace = original.find_first_not_of(" \t");
        if (firstNonSpace == std::string::npos) {
            return processed;
        }
        const std::string prefix = original.substr(0, firstNonSpace);
        return prefix + processed;
    }

    std::filesystem::path DosPathResolver::clampRelativeToBase(const std::filesystem::path &base,
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

    std::string DosPathResolver::resolveMountLinePaths(const std::string &line,
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

    std::string DosPathResolver::sanitizeDosboxMountPath(const std::string &path) {
        // Replicates the Python logic provided, keeping behavior consistent.
        std::string targetDrive;
        std::string targetPath;
        std::string command = "imgmount";

        // Try to find imgmount first
        auto pos = path.find("imgmount");

        // If there is no match, we find mount
        if (pos == std::string::npos) {
            pos = path.find("mount");
            command = "mount";
            if (pos == std::string::npos) {
                return path;
            }
        }

        const std::string mountParams = path.substr(pos + command.length()); // characters after "mount"

        bool startParsing = false;
        std::size_t lastIndex = std::string::npos;

        // Parse targetDrive (the first non-space token after "mount")
        for (std::size_t idx = 0; idx < mountParams.size(); ++idx) {
            const char c = mountParams[idx];

            if (c != ' ')
                startParsing = true;

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
            const std::string pathParams =
                    (lastIndex + 1 < mountParams.size()) ? mountParams.substr(lastIndex + 1) : std::string{};

            startParsing = false;
            for (const char c: pathParams) {
                if (c != ' ')
                    startParsing = true;
                if (startParsing)
                    targetPath.push_back(c);
            }

            if (!targetPath.empty() && targetPath.front() != '"') {
                targetPath = "\"" + targetPath + "\"";
            }
        }

        return command + " " + targetDrive + " " + targetPath;
    }
} // namespace DosboxStagingReplacer
