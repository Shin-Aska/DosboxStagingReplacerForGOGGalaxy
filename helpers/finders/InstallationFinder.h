//
// Created by Orill on 3/14/2025.
//

#ifndef INSTALLATIONFINDER_H
#define INSTALLATIONFINDER_H

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "../../models/InstallationInfo.h"

namespace DosboxStagingReplacer {

    std::vector<DosboxStagingReplacer::InstallationInfo> getInstalledApplications();
    std::string executeCommand(const std::string &command);
    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords);

#ifdef __linux__
    bool isAptAvailable();
    bool isDpkgAvailable();
    bool isRpmAvailable();
    bool isFlatpakAvailable();
    bool isSnapAvailable();

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplications(const std::string &commands, const std::string &source);
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromApt();
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromDpkg();
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromRpm();
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromFlatpak();
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromSnap();
#endif

    class InstallationFinder {
    public:
        void static isApplicationInstalled(const std::string &applicationName);
    };
} // DosboxStagingReplacer

#endif //INSTALLATIONFINDER_H
