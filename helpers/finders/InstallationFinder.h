//
// Created by Richard Orilla on 3/14/2025.
//

#ifndef INSTALLATIONFINDER_H
#define INSTALLATIONFINDER_H

#include <string>
#include <memory>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace DosboxStagingReplacer {

    /*
    * InstallationInfo struct. Contains the information about an installed application
    */
    struct InstallationInfo {
        std::string applicationName;
        std::string installationPath;
        std::string source;
    };

    // Gets all installed applications
    // @param void
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getInstalledApplications();
    // Executes a command and returns the output using pipes
    // @param const std::string &command
    // @return std::string
    std::string executeCommand(const std::string &command);
    // Checks if a string contains all the keywords
    // @param const std::string &text
    // @param const std::vector<std::string> &keywords
    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords);

#ifdef __linux__
    // Checks if apt is available
    // @param void
    // @return bool
    bool isAptAvailable();
    // Checks if dpkg is available
    // @param void
    // @return bool
    bool isDpkgAvailable();
    // Checks if rpm is available
    // @param void
    // @return bool
    bool isRpmAvailable();
    // Checks if flatpak is available
    // @param void
    // @return bool
    bool isFlatpakAvailable();
    // Checks if snap is available
    // @param void
    // @return bool
    bool isSnapAvailable();

    // Gets all registered applications using executeCommand
    // @param const std::string &commands
    // @param const std::string &source
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplications(const std::string &commands, const std::string &source);
    // Gets all registered applications from apt
    // @param void
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromApt();
    // Gets all registered applications from dpkg
    // @param void
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromDpkg();
    // Gets all registered applications from rpm
    // @param void
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromRpm();
    // Gets all registered applications from flatpak
    // @param void
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromFlatpak();
    // Gets all registered applications from snap
    // @param void
    // @return std::vector<DosboxStagingReplacer::InstallationInfo>
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromSnap();
#endif

    /**
     *  InstallationFinder is a utility class that provides the functionality to find an application in the system
     */
    class InstallationFinder {
    public:
        // Finds an application by name in the registered applications in the system
        // @param const std::string &applicationName
        // @return std::vector<DosboxStagingReplacer::InstallationInfo>
        std::vector<DosboxStagingReplacer::InstallationInfo> static findApplication(const std::string &applicationName);
    };
} // DosboxStagingReplacer

#endif //INSTALLATIONFINDER_H
