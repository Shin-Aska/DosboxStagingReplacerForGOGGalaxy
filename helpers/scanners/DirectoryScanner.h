//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H

#include <string>
#include <vector>
#include <filesystem>

#include "CoreHelperModels.h"

namespace DosboxStagingReplacer {

    /**
     * @brief Base class for scanning a directory.
     *
     * DirectoryScanner is meant to be inherited and provides a framework
     * for listing file entities in a specified path.
     */
    class DirectoryScanner {
    public:
        /**
         * @brief Scans a directory and returns a list of files.
         * @param path The path to the directory.
         * @return A list of FileEntity objects found in the directory.
         */
        static std::vector<FileEntity> scanDirectory(const std::string &path);
    };

} // namespace DosboxStagingReplacer

#endif // DIRECTORYSCANNER_H
