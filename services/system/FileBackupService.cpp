//
// Created by Orill on 4/11/2025.
//

#include "FileBackupService.h"
#include "DirectoryScanner.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace DosboxStagingReplacer {

    void FileBackupService::setBackupFileExtension(const std::string& extension) {
        backupFileExtension = extension;
    }

    std::string FileBackupService::getBackupFileExtension() {
        return backupFileExtension;
    }

    void FileBackupService::createBackup(const std::string& filePath, const std::vector<FileEntity> &filesInPath) const {
        auto filesInDirectory = filesInPath;
        // Check if filesInPath is empty
        if (filesInDirectory.empty()) {
            DirectoryScanner scanner;
            const std::string directoryPath = filePath.substr(0, filePath.find_last_of("\\/"));
            filesInDirectory = scanner.scanDirectory(directoryPath);
        }

        for (const auto& file : filesInDirectory) {
            if (file.path == filePath) {
                int backupCounter = 2;
                std::string backupFilePath = file.path + backupFileExtension;
                // Check if the backup file already exists, if so we increment the counter
                // The file will be named file.bak, file.bak.2, file.bak.3, etc.
                // Assuming the backup file extension is ".bak"
                while (fileExists(backupFilePath)) {
                    backupFilePath = file.path + backupFileExtension + std::to_string(backupCounter);
                    backupCounter++;
                }
                // Copy the file to the backup file
                try {
                    std::filesystem::copy_file(file.path, backupFilePath, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "Backup created: " << backupFilePath << std::endl;
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Error creating backup: " << e.what() << std::endl;
                }
            }
        }
    }

    void FileBackupService::restoreFromBackup(const std::string &filePath, const std::vector<FileEntity> filesInPath) {
        auto filesInDirectory = filesInPath;
        // Check if filesInPath is empty
        if (filesInDirectory.empty()) {
            DirectoryScanner scanner;
            const std::string directoryPath = filePath.substr(0, filePath.find_last_of("\\/"));
            filesInDirectory = scanner.scanDirectory(directoryPath);
        }

        if (DosboxStagingReplacer::FileBackupService::backupExists(filePath, filesInPath)) {
            // We first get all possible backup files
            std::vector<FileEntity> backupFiles;
            std::ranges::copy_if(filesInDirectory, std::back_inserter(backupFiles), [&](const auto& file) {
                return file.path.find(filePath) != std::string::npos && file.path.length() > filePath.length();
            });
            // We get the most recent backup file, this is the one with the highest number
            // We assume that the backup file is named file.bak, file.bak.2, file.bak.3, etc.
            // This is done by getting the last number in the file name
            FileEntity mostRecentBackupFile;
            int highestBackupNumber = -1;
            for (const auto& file : backupFiles) {
                std::string fileName = file.path.substr(file.path.find_last_of("\\/") + 1);
                std::string backupNumberString = fileName.substr(fileName.find_last_of(".") + 1);
                if (const int backupNumber = std::stoi(backupNumberString); backupNumber > highestBackupNumber) {
                    highestBackupNumber = backupNumber;
                    mostRecentBackupFile = file;
                }
            }
        }
    }

    bool FileBackupService::backupExists(const std::string& filePath, const std::vector<FileEntity> &filesInPath) {
        auto filesInDirectory = filesInPath;
        // Check if filesInPath is empty
        if (filesInDirectory.empty()) {
            DirectoryScanner scanner;
            const std::string directoryPath = filePath.substr(0, filePath.find_last_of("\\/"));
            filesInDirectory = scanner.scanDirectory(directoryPath);
        }

        // We check if the file exists in the directory, to do this we check if filePath is a substring of file.path
        // and if the length of file.path is greater than filePath, then we know that the file is a backup
        // This is also to cover other .bak files such as .bak.2, .bak.3, etc.
        return std::ranges::any_of(filesInDirectory, [&](const auto& file) {
            return file.path.find(filePath) != std::string::npos && file.path.length() > filePath.length();
        });
    }
}
