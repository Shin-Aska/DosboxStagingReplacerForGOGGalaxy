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

    // Check if a file exists
    // @param std::string path: The path to the file to check
    // @return bool
    bool fileExists(std::string path);

    /**
     *  InstallationVerifier class meant to be used as a base class for other installation verifiers. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for verifying an installation
     */
    class InstallationVerifier {
    private:
        std::string installationPath;
        std::string targetExecutable;
    public:
        explicit InstallationVerifier(std::string installationPath, std::string targetExecutable);
        ~InstallationVerifier();

        // Check if the installation is valid. This a virtual function that has built-in implementation
        // but can be overridden by the child class
        // @param void
        // @return bool
        virtual bool isInstallationValid();
        // Set the installation path of the application to be verified
        // @param std::string installationPath
        // @return void
        void setInstallationPath(std::string installationPath);
        // Set the target executable of the application to be verified
        // @param std::string targetExecutable
        // @return void
        void setTargetExecutable(std::string targetExecutable);
        // Get the installation path of the application to be verified
        std::string getInstallationPath() { return installationPath; }
        // Get the target executable of the application to be verified
        std::string getTargetExecutable() { return targetExecutable; }
    };

    /**
     *  DosboxVerifier class is the implementation of the InstallationVerifier class for the Dosbox application.
     */
    class DosboxVerifier: public InstallationVerifier {

        using InstallationVerifier::InstallationVerifier;

    public:
        // Checks if Dosbox installation is valid
        // @param void
        // @return bool
        bool isInstallationValid() override;
    };

} // DosboxStagingReplacer

#endif //INSTALLATIONVERIFIER_H
