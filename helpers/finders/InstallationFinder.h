//
// Created by Richard Orilla on 3/14/2025.
//

#ifndef INSTALLATIONFINDER_H
#define INSTALLATIONFINDER_H

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace DosboxStagingReplacer {

    /*
     * InstallationInfo struct. Contains the information about an installed application
     * Note: When updating the struct, make sure to update DataExporter::stringify as well
     */
    struct InstallationInfo {
        std::string applicationName;
        std::string installationPath;
        std::string source;
    };

    /**
     * @brief Utility class that provides the functionality to find an application in the system.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getInstalledApplications();
    /**
     * @brief Utility function to check if a command is available in the system.
     * @param command The command to check.
     * @return True if the command is available, false otherwise.
     */
    std::string executeCommand(const std::string &command);
    /**
     * @brief Utility function to match strings from a set of keywords.
     * @param text The text to search in.
     * @param keywords The keywords to search for.
     */
    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords);

#ifdef __linux__
    /**
     * @brief Utility function specific to Linux to check if Apt is installed in the system.
     */
    bool isAptAvailable();
    /**
     * @brief Utility function specific to Linux to check if Dpkg is installed in the system.
     */
    bool isDpkgAvailable();
    /**
     * @brief Utility function specific to Linux to check if Rpm is installed in the system.
     */
    bool isRpmAvailable();
    /**
     * @brief Utility function specific to Linux to check if Flatpak is installed in the system.
     */
    bool isFlatpakAvailable();
    /**
     * @brief Utility function specific to Linux to check if Snap is installed in the system.
     */
    bool isSnapAvailable();
    /**
     * @brief Utility function specific to Linux to obtain the list of installed applications.
     * @param commands The command to execute.
     * @param source The source of the application
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplications(const std::string &commands,
                                                                                   const std::string &source);
    /**
     * @brief Utility function specific to Linux to obtain the list of installed applications from Apt.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromApt();
    /**
     * @brief Utility function specific to Linux to obtain the list of installed applications from Dpkg.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromDpkg();
    /**
     * @brief Utility function specific to Linux to obtain the list of installed applications from Rpm.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromRpm();
    /**
     * @brief Utility function specific to Linux to obtain the list of installed applications from Flatpak.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromFlatpak();
    /**
     * @brief Utility function specific to Linux to obtain the list of installed applications from Snap.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromSnap();
#elif _WIN32
    /**
     * @brief Utility function specific to Windows to obtain the list of installed applications.
     * @return A vector of InstallationInfo objects containing the information about the installed applications.
     */
    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromWinApi();
#endif

    /**
     * @brief Utility class that provides the functionality to find an application in the system.
     */
    class InstallationFinder {
    public:
        /**
         * @brief Finds an application by name in the registered applications in the system.
         * @param applicationName The name of the application to search for.
         * @return A vector of InstallationInfo objects matching the application name.
         */
        static std::vector<DosboxStagingReplacer::InstallationInfo> findApplication(const std::string &applicationName);
    };
} // namespace DosboxStagingReplacer

#endif // INSTALLATIONFINDER_H
