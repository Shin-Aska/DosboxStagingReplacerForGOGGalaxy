//
// Created by Orill on 3/14/2025.
//

#ifndef INSTALLATIONFINDER_H
#define INSTALLATIONFINDER_H

namespace DosboxStagingReplacer {
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../../models/InstallationInfo.h"

    std::vector<DosboxStagingReplacer::InstallationInfo> getInstalledApplications();
    std::vector<std::string> getFilesInDirectory(const std::string &path);
    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords);

    class InstallationFinder {
    public:
        void static isApplicationInstalled(const std::string &applicationName);
    };
} // DosboxStagingReplacer

#endif //INSTALLATIONFINDER_H
