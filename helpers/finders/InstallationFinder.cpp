//
// Created by Richard Orilla on 3/14/2025.
//

#include "InstallationFinder.h"

namespace DosboxStagingReplacer {

    // Platform specific code first
#ifdef __linux__
    // Originally I want to use libraries like libapt, libdpkg, librpm, libflatpak, and libsnapd
    // But I think it's better to use the command line tools instead as it prevents the need to install additional libraries
    // Furthermore it will be more portable as it will work on any Linux distribution
    #define DELIMITER '\t'

    #define APT "apt"
    #define FLATPAK "flatpak"
    #define SNAP "snap"
    #define DPKG "dpkg"
    #define RPM "rpm"

    #define APT_COMMAND "apt list --installed 2>/dev/null | awk -F/ 'NR>1 {print $1}' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{}\t$p\tapt\"'"
    #define FLATPAK_COMMAND "flatpak list --app --columns=application | tail -n +1 | xargs -I{} sh -c 'p=$(flatpak info --show-location {} 2>/dev/null); echo \"{}\t$p\tflatpak\"'"
    #define SNAP_COMMAND "snap list | awk 'NR>1 {print $1}' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{},$p,snap\"'"
    #define DPKG_COMMAND "dpkg -l | awk 'NR>5 {print $2}' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{}\t$p\tdpkg\"'"
    #define RPM_COMMAND "rpm -qa --qf '%{NAME}\n' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{},$p,rpm\"'"


    bool isAptAvailable() {
        return !executeCommand("command -v apt").empty();
    }

    bool isFlatpakAvailable() {
        return !executeCommand("command -v flatpak").empty();
    }

    bool isSnapAvailable() {
        return !executeCommand("command -v snap").empty();
    }

    bool isDpkgAvailable() {
        return !executeCommand("command -v dpkg").empty();
    }

    bool isRpmAvailable() {
        return !executeCommand("command -v rpm").empty();
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplications(const std::string &commands, const std::string &source) {
        std::vector<DosboxStagingReplacer::InstallationInfo> registeredApplications;
        std::string output = executeCommand(commands);
        // Get all lines from the output and process each line
        std::istringstream stream(output);
        std::string line;
        while (std::getline(stream, line)) {
            // Split the line by the tab character
            std::istringstream lineStream(line);
            std::string applicationName;
            std::string installationPath;
            std::getline(lineStream, applicationName, DELIMITER);
            std::getline(lineStream, installationPath, DELIMITER);
            DosboxStagingReplacer::InstallationInfo info;
            info.applicationName = applicationName;
            info.installationPath = installationPath;
            info.source = source;
            registeredApplications.push_back(info);
        }
        return registeredApplications;
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromApt() {
        return getRegisteredApplications(APT_COMMAND, APT);
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromFlatpak() {
        return getRegisteredApplications(FLATPAK_COMMAND, FLATPAK);
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromSnap() {
        return getRegisteredApplications(SNAP_COMMAND, SNAP);
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromDpkg() {
        return getRegisteredApplications(DPKG_COMMAND, DPKG);
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> getRegisteredApplicationsFromRpm() {
        return getRegisteredApplications(RPM_COMMAND, RPM);
    }

#endif

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
                    info.source = "Registry";
                    result.push_back(info);
                }
                RegCloseKey(hSubKey);
            }

            // Reset subkey size and increment index
            subKeySize = std::size(subKeyName);
            index++;
        }

        RegCloseKey(hKey);
#elif __linux__
        // Apt logic code, also if dpkg is available as well (because dpkg is a dependency of apt)
        // Apt will be used to get the list of installed applications
        if (isAptAvailable() || isDpkgAvailable()) {
            if (isAptAvailable()) {
                auto apt_apps = getRegisteredApplicationsFromApt();
                result.insert(result.end(), apt_apps.begin(), apt_apps.end());
            }
            else {
                auto dpkg_apps = getRegisteredApplicationsFromDpkg();
                result.insert(result.end(), dpkg_apps.begin(), dpkg_apps.end());
            }
        }
        if (isRpmAvailable()) {
            auto rpm_apps = getRegisteredApplicationsFromRpm();
            result.insert(result.end(), rpm_apps.begin(), rpm_apps.end());
        }
        if (isFlatpakAvailable()) {
            auto flatpak_apps = getRegisteredApplicationsFromFlatpak();
            result.insert(result.end(), flatpak_apps.begin(), flatpak_apps.end());
        }
        if (isSnapAvailable()) {
            auto snap_apps = getRegisteredApplicationsFromSnap();
            result.insert(result.end(), snap_apps.begin(), snap_apps.end());
        }

#endif
        return result;
    }

    std::string executeCommand(const std::string &command) {
        std::array<char, 512> buffer {};
        std::string result;

        const std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords) {
        std::string lowerText = text;
        std::ranges::transform(lowerText, lowerText.begin(), ::tolower);
        int matchCount = 0;
        for (const auto &keyword : keywords) {
            // Create a copy of keyword and store it to k
            std::string k = keyword;
            std::ranges::transform(k, k.begin(), ::tolower);
            if (lowerText.find(keyword) != std::string::npos) {
                matchCount += 1;
            }
        }
        return keywords.size() == matchCount;
    }

    std::vector<DosboxStagingReplacer::InstallationInfo> InstallationFinder::findApplication(const std::string &applicationName) {
        std::vector<DosboxStagingReplacer::InstallationInfo> result;
        for (auto installedApps = getInstalledApplications(); auto &app : installedApps) {
            // Split the application name by spaces and then consider that as keywords to fed to lazyStringMatching
            std::istringstream iss(applicationName);
            std::vector<std::string> keywords;
            std::string keyword;
            while (iss >> keyword) {
                keywords.push_back(keyword);
            }

            // Check if the application name contains all the keywords and if so, add it to the result
            if (lazyStringMatching(app.applicationName, keywords)) {
                result.push_back(app);
            }
        }
        return result;
    }
} // DosboxStagingReplacer
