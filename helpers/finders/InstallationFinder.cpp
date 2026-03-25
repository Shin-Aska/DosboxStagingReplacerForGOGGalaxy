//
// Created by Richard Orilla on 3/14/2025.
//

#include "InstallationFinder.h"
#include <unordered_set>

namespace DosboxStagingReplacer {

    // Platform specific code first
#ifdef __linux__
    // Originally I want to use libraries like libapt, libdpkg, librpm, libflatpak, and libsnapd
    // But I think it's better to use the command line tools instead as it prevents the need to install additional libraries
    // Furthermore it will be more portable as it will work on any Linux distribution
    constexpr char DELIMITER = '\t';

    constexpr const char *APT = "apt";
    constexpr const char *FLATPAK = "flatpak";
    constexpr const char *SNAP = "snap";
    constexpr const char *DPKG = "dpkg";
    constexpr const char *RPM = "rpm";

    constexpr const char *APT_COMMAND = "apt list --installed 2>/dev/null | awk -F/ 'NR>1 {print $1}' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{}\t$p\tapt\"'";
    constexpr const char *FLATPAK_COMMAND = "flatpak list --app --columns=application | tail -n +1 | xargs -I{} sh -c 'p=$(flatpak info --show-location {} 2>/dev/null); echo \"{}\t$p\tflatpak\"'";
    constexpr const char *SNAP_COMMAND = "snap list | awk 'NR>1 {print $1}' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{},$p,snap\"'";
    constexpr const char *DPKG_COMMAND = "dpkg -l | awk 'NR>5 {print $2}' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{}\t$p\tdpkg\"'";
    constexpr const char *RPM_COMMAND = "rpm -qa --qf '%{NAME}\n' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{},$p,rpm\"'";

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

    std::vector<InstallationInfo> getRegisteredApplications(const std::string &commands, const std::string &source) {
        std::vector<InstallationInfo> registeredApplications;
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
            InstallationInfo info;
            info.applicationName = applicationName;
            info.installationPath = installationPath;
            info.source = source;
            registeredApplications.push_back(info);
        }
        return registeredApplications;
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromApt() {
        return getRegisteredApplications(APT_COMMAND, APT);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromFlatpak() {
        return getRegisteredApplications(FLATPAK_COMMAND, FLATPAK);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromSnap() {
        return getRegisteredApplications(SNAP_COMMAND, SNAP);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromDpkg() {
        return getRegisteredApplications(DPKG_COMMAND, DPKG);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromRpm() {
        return getRegisteredApplications(RPM_COMMAND, RPM);
    }

#elif _WIN32

    std::string readRegistryStringValue(HKEY key, const char *valueName) {
        TCHAR buffer[1024];
        DWORD type = 0;
        DWORD size = sizeof(buffer);
        if (RegQueryValueEx(key, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &size) ==
                ERROR_SUCCESS &&
            type == REG_SZ) {
            return buffer;
        }
        return {};
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsRegistryView(const HKEY hive,
                                                                                    REGSAM samFlags) {
        std::vector<InstallationInfo> result;
        constexpr auto uninstallKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
        HKEY hKey = nullptr;
        if (RegOpenKeyEx(hive, uninstallKey, 0, KEY_READ | samFlags, &hKey) != ERROR_SUCCESS) {
            return result;
        }

        DWORD index = 0;
        TCHAR subKeyName[256];
        DWORD subKeySize = std::size(subKeyName);

        while (RegEnumKeyEx(hKey, index, subKeyName, &subKeySize, nullptr, nullptr, nullptr, nullptr) ==
               ERROR_SUCCESS) {
            HKEY hSubKey = nullptr;

            if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ | samFlags, &hSubKey) == ERROR_SUCCESS) {
                const std::string displayName = readRegistryStringValue(hSubKey, "DisplayName");
                if (!displayName.empty()) {
                    auto installationPath = readRegistryStringValue(hSubKey, "InstallLocation");
                    if (installationPath.empty()) {
                        installationPath = readRegistryStringValue(hSubKey, "UninstallString");
                    }
                    if (installationPath.empty()) {
                        installationPath = "Unknown";
                    }

                    InstallationInfo info;
                    info.applicationName = displayName;
                    info.installationPath = installationPath;
                    info.source = "Registry";
                    result.push_back(info);
                }
                RegCloseKey(hSubKey);
            }

            subKeySize = std::size(subKeyName);
            index++;
        }
        RegCloseKey(hKey);
        return result;
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsMachine64() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsMachine32() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsUser64() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_CURRENT_USER, KEY_WOW64_64KEY);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsUser32() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_CURRENT_USER, KEY_WOW64_32KEY);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindows() {
        std::vector<InstallationInfo> result;
        std::unordered_set<std::string> seenEntries;
        const std::array registryViews = {
                getRegisteredApplicationsFromWindowsMachine64(),
                getRegisteredApplicationsFromWindowsMachine32(),
                getRegisteredApplicationsFromWindowsUser64(),
                getRegisteredApplicationsFromWindowsUser32(),
        };

        for (const auto &entries : registryViews) {
            for (const auto &entry : entries) {
                const auto dedupeKey = entry.applicationName + '\n' + entry.installationPath;
                if (seenEntries.insert(dedupeKey).second) {
                    result.push_back(entry);
                }
            }
        }

        return result;
    }

#endif

    std::vector<InstallationInfo> getInstalledApplications() {
        auto result = std::vector<InstallationInfo>();
#ifdef _WIN32
        auto win32_apps = getRegisteredApplicationsFromWindows();
        result.insert(result.end(), win32_apps.begin(), win32_apps.end());
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
        std::array<char, 512> buffer{};
        std::string result;

        const std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += std::string(buffer.data());
        }
        return result;
    }

    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords) {
        std::string lowerText = text;
        std::ranges::transform(lowerText, lowerText.begin(), tolower);
        int matchCount = 0;
        for (const auto &keyword : keywords) {
            // Create a copy of keyword and store it to k
            std::string k = keyword;
            std::ranges::transform(k, k.begin(), tolower);
            if (lowerText.find(keyword) != std::string::npos) {
                matchCount += 1;
            }
        }
        return keywords.size() == matchCount;
    }

    std::vector<InstallationInfo> InstallationFinder::findApplication(const std::string &applicationName) {
        std::vector<InstallationInfo> result;
        for (auto installedApps = getInstalledApplications(); auto &app : installedApps) {
            // Lowercase the application name for case-insensitive comparison
            std::string lowerAppName = applicationName;
            std::ranges::transform(lowerAppName, lowerAppName.begin(), tolower);
            // Split the application name by spaces and then consider that as keywords to fed to lazyStringMatching
            std::istringstream iss(lowerAppName);
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
