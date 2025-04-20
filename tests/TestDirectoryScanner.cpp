#include <iostream>
#include "DirectoryScanner.h"

int main () {
    const auto files = DosboxStagingReplacer::DirectoryScanner::scanDirectory("../tests/data");
    // Check if in the files there is an entry called invalid.sqlite and folder, invalid.sqlite must be a file
    // while folder must be a directory
    std::cout << "Expecting at least a file named invalid.sqlite and a directory named folder" << std::endl;
    std::cout << "Found " << files.size() << " records in the target directory" << std::endl;
    bool found_invalid_db = false;
    bool found_folder = false;
    for (const auto &file : files) {
        std::cout << "File: " << file.name << " Type: " << (file.type == DosboxStagingReplacer::FileType::FILE ? "FILE" : "DIRECTORY") << std::endl;
        if (file.name == "invalid.sqlite" && file.type != DosboxStagingReplacer::FileType::FILE) {
            found_invalid_db = true;
        } else if (file.name == "folder" && file.type == DosboxStagingReplacer::FileType::DIRECTORY) {
            found_folder = true;
        }
    }
    if (!found_invalid_db) {
        std::cout << "Did not find invalid.sqlite" << std::endl;
    }
    if (!found_folder) {
        std::cout << "Did not find folder" << std::endl;
    }
    return found_folder && found_invalid_db;
}