//
// Created by Richard Orilla on 3/14/2025.
//

#include "InstallationVerifier.h"

namespace DosboxStagingReplacer {

    bool fileExists(const std::string &path) {
        return std::filesystem::exists(path);
    }

    InstallationVerifier::InstallationVerifier(const std::string &installationPath,
                                               const std::string &targetExecutable) {
        this->installationPath = installationPath;
        this->targetExecutable = targetExecutable;
    }

    InstallationVerifier::~InstallationVerifier() = default;

    bool InstallationVerifier::isInstallationValid() {
        /*
            * Check if the target executable exists in the installation path
         */
        std::string path = installationPath;
        if (path.back() != '/') {
            path += "/";
        }
        path += targetExecutable;
        return fileExists(path);
    }

    void InstallationVerifier::setInstallationPath(const std::string &installationPath) {
        this->installationPath = installationPath;
    }

    void InstallationVerifier::setTargetExecutable(const std::string &targetExecutable) {
        this->targetExecutable = targetExecutable;
    }

    bool DosboxVerifier::isInstallationValid() {
        return InstallationVerifier::isInstallationValid();
    }
} // DosboxStagingReplacer
