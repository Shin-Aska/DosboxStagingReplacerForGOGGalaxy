//
// Created by Orill on 3/14/2025.
//

#ifndef INSTALLATIONVERIFIER_H
#define INSTALLATIONVERIFIER_H

#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace DosboxStagingReplacer {

    bool executableExists(std::string path);

    class InstallationVerifier {
    private:
        std::string installationPath;
        std::string targetExecutable;
    public:
        explicit InstallationVerifier(std::string installationPath, std::string targetExecutable);
        ~InstallationVerifier();

        virtual bool isInstallationValid();
        void setInstallationPath(std::string installationPath);
        void setTargetExecutable(std::string targetExecutable);
        std::string getInstallationPath() { return installationPath; }
        std::string getTargetExecutable() { return targetExecutable; }
    };

    class DosboxVerifier: public InstallationVerifier {

        using InstallationVerifier::InstallationVerifier;

    public:
        bool isInstallationValid() override;
    };

} // DosboxStagingReplacer

#endif //INSTALLATIONVERIFIER_H
