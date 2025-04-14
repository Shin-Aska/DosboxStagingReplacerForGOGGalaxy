//
// Created by Richard Orilla on 3/23/2025.
//

#include "DirectoryScanner.h"

namespace DosboxStagingReplacer {

    std::vector<FileEntity> DirectoryScanner::scanDirectory(const std::string& path) {

        auto files = std::vector<FileEntity>();
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            FileEntity file;
            file.name = entry.path().filename().string();
            file.path = entry.path().string();
            file.size = 0;
            file.type = FileType::FILE;
            if (entry.is_directory())
                file.type = FileType::DIRECTORY;
            else
                file.size = entry.file_size();
            files.push_back(file);
        }
        return files;
    }

} // DosboxStagingReplacer
