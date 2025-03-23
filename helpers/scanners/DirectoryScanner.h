//
// Created by Orill on 3/23/2025.
//

#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H

namespace DosboxStagingReplacer {

#include <string>
#include <vector>
#include "../../models/FileEntity.h"

    class DirectoryScanner {
    public:
        std::vector<DosboxStagingReplacer::FileEntity> scanDirectory(std::string path);
    };

} // DosboxStagingReplacer

#endif //DIRECTORYSCANNER_H
