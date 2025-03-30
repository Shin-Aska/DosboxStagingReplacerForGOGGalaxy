//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H

namespace DosboxStagingReplacer {

#include <string>
#include <vector>
#include "../../models/FileEntity.h"

    /**
     *  DirectoryScanner class meant to be used as a base class for other directory scanners. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for scanning a directory
     */
    class DirectoryScanner {
    public:
        // Scans a directory and returns a list of files
        // @param path The path to the directory
        // @return A list of files in the directory
        std::vector<DosboxStagingReplacer::FileEntity> scanDirectory(std::string path);
    };

} // DosboxStagingReplacer

#endif //DIRECTORYSCANNER_H
