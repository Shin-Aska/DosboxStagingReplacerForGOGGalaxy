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
        FILE = 1,
        NONE = -1,
    };

    /*
     * FileEntity struct. Contains the information about a file
     */
    class FileEntity {
    public:

        std::string name;
        std::string path;
        FileType type;
        unsigned long size;

        explicit FileEntity(const std::string &name = "", const std::string &path = "", const FileType type = FileType::NONE,
                   const unsigned long size = 0) : type(), size(0) {
            this->name = name;
            this->path = path;
            this->type = type;
            this->size = size;
        }

        [[nodiscard]] bool isFile() const {
            return type == FileType::FILE;
        }

        [[nodiscard]] bool isDirectory() const {
            return type == FileType::DIRECTORY;
        }

        [[nodiscard]] bool isNull() const {
            return type == FileType::NONE;
        }
    };

} // namespace DosboxStagingReplacer



#endif //COREHELPERMODELS_H
