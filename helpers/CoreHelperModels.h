//
// Created by Orill on 4/11/2025.
//

#ifndef COREHELPERMODELS_H
#define COREHELPERMODELS_H

#include <string>

namespace DosboxStagingReplacer {

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

} // namespace DosboxStagingReplacer



#endif //COREHELPERMODELS_H
