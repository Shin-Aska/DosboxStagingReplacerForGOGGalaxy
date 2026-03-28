//
// Created by Richard Orilla on 3/14/2025.
//

#include "InstallationFinder.h"
#include <unordered_set>

namespace DosboxStagingReplacer {

    // Platform specific code first
#ifdef __linux__
    // Originally I want to use libraries like libapt, libdpkg, librpm, libflatpak, and libsnapd
    // But I think it's better to use the command line tools instead as it prevents the need to install additional
    // libraries Furthermore it will be more portable as it will work on any Linux distribution
    constexpr char DELIMITER = '\t';

    constexpr const char *APT = "apt";
    constexpr const char *FLATPAK = "flatpak";
    constexpr const char *SNAP = "snap";
    constexpr const char *DPKG = "dpkg";
    constexpr const char *RPM = "rpm";

    constexpr const char *APT_COMMAND = "apt list --installed 2>/dev/null | awk -F/ 'NR>1 {print $1}' | xargs -I{} sh "
                                        "-c 'p=$(command -v {} 2>/dev/null); echo \"{}\t$p\tapt\"'";
    constexpr const char *FLATPAK_COMMAND =
            "flatpak list --app --columns=application | tail -n +1 | xargs -I{} sh -c 'p=$(flatpak info "
            "--show-location {} 2>/dev/null); echo \"{}\t$p\tflatpak\"'";
    constexpr const char *SNAP_COMMAND = "snap list | awk 'NR>1 {print $1}' | xargs -I{} sh -c 'p=$(command -v {} "
                                         "2>/dev/null); echo \"{},$p,snap\"'";
    constexpr const char *DPKG_COMMAND = "dpkg -l | awk 'NR>5 {print $2}' | xargs -I{} sh -c 'p=$(command -v {} "
                                         "2>/dev/null); echo \"{}\t$p\tdpkg\"'";
    constexpr const char *RPM_COMMAND =
            "rpm -qa --qf '%{NAME}\n' | xargs -I{} sh -c 'p=$(command -v {} 2>/dev/null); echo \"{},$p,rpm\"'";

    bool isAptAvailable() { return !executeCommand("command -v apt").empty(); }

    bool isFlatpakAvailable() { return !executeCommand("command -v flatpak").empty(); }

    bool isSnapAvailable() { return !executeCommand("command -v snap").empty(); }

    bool isDpkgAvailable() { return !executeCommand("command -v dpkg").empty(); }

    bool isRpmAvailable() { return !executeCommand("command -v rpm").empty(); }

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

    std::string toUtf8(const std::wstring &wstr) {
        if (wstr.empty())
            return {};

        const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.data(), static_cast<int>(wstr.size()),
                                             nullptr, 0, nullptr, nullptr);

        if (size <= 0)
            return {};

        std::string result(size, '\0');

        const int written = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.data(),
                                                static_cast<int>(wstr.size()), result.data(), size, nullptr, nullptr);

        if (written != size)
            return {};

        return result;
    }

    std::wstring readRegistryStringValueW(HKEY key, const wchar_t *valueName) {
        DWORD type = 0;
        DWORD size = 0;
        constexpr DWORD flags = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_ZEROONFAILURE;

        if (RegGetValueW(key, nullptr, valueName, flags, &type, nullptr, &size) != ERROR_SUCCESS || size == 0) {
            return {};
        }

        std::wstring value(size / sizeof(wchar_t), L'\0');

        if (RegGetValueW(key, nullptr, valueName, flags, &type, value.data(), &size) != ERROR_SUCCESS) {
            return {};
        }

        while (!value.empty() && value.back() == L'\0') {
            value.pop_back();
        }

        return value;
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsRegistryView(HKEY hive, REGSAM samFlags) {
        std::vector<InstallationInfo> result;

        HKEY hKey = nullptr;
        if (RegOpenKeyExW(hive, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ | samFlags,
                          &hKey) != ERROR_SUCCESS) {
            return result;
        }

        DWORD subKeyCount = 0;
        DWORD maxSubKeyLen = 0;
        if (RegQueryInfoKeyW(hKey, nullptr, nullptr, nullptr, &subKeyCount, &maxSubKeyLen, nullptr, nullptr, nullptr,
                             nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return result;
        }

        std::wstring subKeyName(maxSubKeyLen + 1, L'\0');

        for (DWORD index = 0; index < subKeyCount; ++index) {
            auto subKeySize = static_cast<DWORD>(subKeyName.size());
            const LSTATUS status =
                    RegEnumKeyExW(hKey, index, subKeyName.data(), &subKeySize, nullptr, nullptr, nullptr, nullptr);
            if (status != ERROR_SUCCESS) {
                continue;
            }

            HKEY hSubKey = nullptr;

            if (std::wstring_view nameView(subKeyName.data(), subKeySize);
                RegOpenKeyExW(hKey, std::wstring(nameView).c_str(), 0, KEY_READ | samFlags, &hSubKey) ==
                ERROR_SUCCESS) {
                auto displayNameW = readRegistryStringValueW(hSubKey, L"DisplayName");
                if (!displayNameW.empty()) {
                    auto installLocationW = readRegistryStringValueW(hSubKey, L"InstallLocation");
                    auto uninstallStringW = readRegistryStringValueW(hSubKey, L"UninstallString");

                    InstallationInfo info;
                    info.applicationName = toUtf8(displayNameW);
                    info.installationPath =
                            !installLocationW.empty()
                                    ? toUtf8(installLocationW)
                                    : (!uninstallStringW.empty() ? toUtf8(uninstallStringW) : "Unknown");
                    info.source = "Registry";
                    result.push_back(std::move(info));
                }

                RegCloseKey(hSubKey);
            }
        }

        RegCloseKey(hKey);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsMachine64() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsMachine32() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindowsUser() {
        return getRegisteredApplicationsFromWindowsRegistryView(HKEY_CURRENT_USER, 0);
    }

    std::vector<InstallationInfo> getRegisteredApplicationsFromWindows() {
        std::vector<InstallationInfo> result;
        std::unordered_set<std::string> seenEntries;
        const std::array registryViews = {
                getRegisteredApplicationsFromWindowsMachine64(),
                getRegisteredApplicationsFromWindowsMachine32(),
                getRegisteredApplicationsFromWindowsUser(),
        };

        for (const auto &entries: registryViews) {
            for (const auto &entry: entries) {
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
            } else {
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
        if (!pipe)
            throw std::runtime_error("popen() failed!");

        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += std::string(buffer.data());
        }
        return result;
    }

    bool lazyStringMatching(const std::string &text, const std::vector<std::string> &keywords) {
        std::string lowerText = text;
        std::ranges::transform(lowerText, lowerText.begin(), tolower);
        int matchCount = 0;
        for (const auto &keyword: keywords) {
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
        for (auto installedApps = getInstalledApplications(); auto &app: installedApps) {
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
} // namespace DosboxStagingReplacer
