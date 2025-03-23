//
// Created by Orill on 3/14/2025.
//

#ifndef INSTALLATIONFINDER_CPP
#define INSTALLATIONFINDER_CPP

#include "InstallationFinder.h"
#include <algorithm>

namespace DosboxStagingReplacer {
    std::vector<DosboxStagingReplacer::InstallationInfo> getInstalledApplications() {
        auto result = std::vector<DosboxStagingReplacer::InstallationInfo>();
#ifdef _WIN32
        HKEY hKey;
        DWORD index = 0;
        TCHAR subKeyName[256];
        DWORD subKeySize = std::size(subKeyName);

        // Open the uninstall registry key
        if (auto uninstallKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
            RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstallKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            std::cerr << "Failed to open registry key." << std::endl;
        }

        // Enumerate subkeys (each subkey represents an installed program)
        while (RegEnumKeyEx(hKey, index, subKeyName, &subKeySize, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
            HKEY hSubKey;
            TCHAR displayName[256];
            TCHAR installLocation[512];
            DWORD displaySize = sizeof(displayName);
            DWORD installSize = sizeof(installLocation);
            DWORD type;

            // Open each subkey
            if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                // Query the "DisplayName" value
                if (RegQueryValueEx(hSubKey, "DisplayName", nullptr, &type, reinterpret_cast<LPBYTE>(displayName), &displaySize) ==
                    ERROR_SUCCESS && type == REG_SZ) {
                    // Query the "InstallLocation" value
                    if (RegQueryValueEx(hSubKey, "InstallLocation", nullptr, &type, reinterpret_cast<LPBYTE>(installLocation), &installSize)
                        != ERROR_SUCCESS || type != REG_SZ) {
                        // If InstallLocation is missing, try "UninstallString" (which often contains the install path)
                        installSize = sizeof(installLocation);
                        if (RegQueryValueEx(hSubKey, "UninstallString", nullptr, &type, reinterpret_cast<LPBYTE>(installLocation),
                                            &installSize) != ERROR_SUCCESS || type != REG_SZ) {
                            // If both values are missing, set a default message
                            lstrcpy(installLocation, "Unknown");
                        }
                    }

                    DosboxStagingReplacer::InstallationInfo info;
                    info.applicationName = displayName;
                    info.installationPath = installLocation;
                    result.push_back(info);
                }
                RegCloseKey(hSubKey);
            }

            // Reset subkey size and increment index
            subKeySize = std::size(subKeyName);
            index++;
        }

        RegCloseKey(hKey);
#endif
        return result;
    }

    std::vector<std::string> getFilesInDirectory(const std::string &path) {
        auto result = std::vector<std::string>();
#ifdef _WIN32
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((path + "\\*").c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            return result;
        }

        do {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            result.push_back(findFileData.cFileName);
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
#endif
        return result;
    }

    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords) {
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
        int matchCount = 0;
        for (const auto &keyword : keywords) {
            // Create a copy of keyword and store it to k
            std::string k = keyword;
            std::transform(k.begin(), k.end(), k.begin(), ::tolower);
            if (lowerText.find(keyword) != std::string::npos) {
                matchCount += 1;
            }
        }
        return keywords.size() == matchCount;
    }

    void InstallationFinder::isApplicationInstalled(const std::string &applicationName) {
        auto installedApps = getInstalledApplications();
        for (auto &app : installedApps) {
            if (lazyStringMatching(app.applicationName, {"dosbox", "staging"})) {
                std::cout << app.applicationName << "\t(" << app.installationPath << ")" << std::endl;
            }
        }
    }
} // DosboxStagingReplacer

#endif //INSTALLATIONFINDER_CPP