//
// Created by Orill on 3/14/2025.
//

#ifndef INSTALLATIONVERIFIER_CPP
#define INSTALLATIONVERIFIER_CPP

#include "InstallationVerifier.h"

namespace DosboxStagingReplacer {

    bool fileExists(const std::string path) {
        /*
            * Check if the file at the given path is an executable
        */
        #ifdef _WIN32
                DWORD fileAttr = GetFileAttributesA(path.c_str());
                if (fileAttr == INVALID_FILE_ATTRIBUTES) {
                    return false;
                }
                return (fileAttr & FILE_ATTRIBUTE_DIRECTORY) == 0;
        #else
                return access(path.c_str(), X_OK) == 0;
        #endif
    }

    InstallationVerifier::InstallationVerifier(std::string installationPath, std::string targetExecutable) {
        this->installationPath = installationPath;
        this->targetExecutable = targetExecutable;
    }

    InstallationVerifier::~InstallationVerifier() {
        // Destructor
    }

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

    void InstallationVerifier::setInstallationPath(std::string installationPath) {
        this->installationPath = installationPath;
    }

    void InstallationVerifier::setTargetExecutable(std::string targetExecutable) {
        this->targetExecutable = targetExecutable;
    }

    bool DosboxVerifier::isInstallationValid() {
        return InstallationVerifier::isInstallationValid();
    }
} // DosboxStagingReplacer

#endif //INSTALLATIONVERIFIER_CPP