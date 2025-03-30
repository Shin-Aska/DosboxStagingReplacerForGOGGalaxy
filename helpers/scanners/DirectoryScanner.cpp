//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef DIRECTORYSCANNER_CPP
#define DIRECTORYSCANNER_CPP

#include "DirectoryScanner.h"
#include <filesystem>

namespace DosboxStagingReplacer {

    std::vector<DosboxStagingReplacer::FileEntity> DirectoryScanner::scanDirectory(std::string path) {

        auto files = std::vector<DosboxStagingReplacer::FileEntity>();
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            DosboxStagingReplacer::FileEntity file;
            file.name = entry.path().filename().string();
            file.path = entry.path().string();
            file.size = 0;
            file.type = DosboxStagingReplacer::FileType::FILE;
            if (entry.is_directory())
                file.type = DosboxStagingReplacer::FileType::DIRECTORY;
            else
                file.size = entry.file_size();
            files.push_back(file);
        }
        return files;
    }

} // DosboxStagingReplacer

#endif //DIRECTORYSCANNER_CPP