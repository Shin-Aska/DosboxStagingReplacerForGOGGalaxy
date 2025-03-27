//
// Created by Orill on 3/23/2025.
//

#ifndef FILEENTITY_H
#define FILEENTITY_H

namespace DosboxStagingReplacer {

#include <string>

    /**
     *  FileType enum class. Lists down the supported file types
     */
    enum class FileType {
        DIRECTORY = 0,
        FILE = 1
    };

    /*
     * FileEntity struct. Contains the information about a file
     */
    struct FileEntity {
        std::string name;
        std::string path;
        FileType type;
        unsigned long size;
    };

} // DosboxStagingReplacer

#endif //FILEENTITY_H
