//
// Created by Orill on 4/16/2025.
//

#include "ScriptEditService.h"
#include "InstallationVerifier.h"

#include <algorithm>
#include <fstream>

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

    std::string ScriptEditService::resolveRelativePathsFromString(const std::string &cmd,
                                                                  const std::filesystem::path &basePath) {
        // Copy cmd to a new variable
        std::string result = cmd;
        // Replace all relative paths with absolute paths
        replaceAll(result, "..", basePath.string());
        return result;
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
                    replaceAll(line, "..", basePath.string());
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

    void ScriptEditService::disableFullScreenForDosboxConfig(std::filesystem::path &filePath, const std::string &tmpExtension) {
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
} // DosboxStagingReplacer