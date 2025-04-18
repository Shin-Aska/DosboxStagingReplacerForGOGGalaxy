//
// Created by Richard Orilla on 3/14/2025.
//

#ifndef INSTALLATIONVERIFIER_H
#define INSTALLATIONVERIFIER_H

#include <string>
#include <fstream>
#include <filesystem>

namespace DosboxStagingReplacer {

    /**
     * @brief Check if a file exists.
     *
     * @param path The path to the file to check.
     * @return True if the file exists, false otherwise.
     */
    bool fileExists(const std::string &path);

    /**
     * @brief Base class for verifying an installation.
     *
     * Meant to be inherited from. Provides a framework for checking if a target application
     * is installed correctly in a given path.
     */
    class InstallationVerifier {
        std::string installationPath;
        std::string targetExecutable;
    public:
        explicit InstallationVerifier(const std::string &installationPath, const std::string &targetExecutable);
        virtual ~InstallationVerifier();

        /**
         * @brief Checks if the installation is valid.
         *
         * Can be overridden by derived classes to implement specific logic.
         *
         * @return True if valid, false otherwise.
         */
        virtual bool isInstallationValid();

        /**
         * @brief Sets the installation path.
         *
         * @param installationPath The new installation path.
         */
        void setInstallationPath(const std::string &installationPath);

        /**
         * @brief Sets the target executable name.
         *
         * @param targetExecutable The executable file name.
         */
        void setTargetExecutable(const std::string &targetExecutable);

        /**
         * @brief Gets the installation path.
         *
         * @return The installation path.
         */
        std::string getInstallationPath() { return installationPath; }

        /**
         * @brief Gets the target executable name.
         *
         * @return The target executable.
         */
        std::string getTargetExecutable() { return targetExecutable; }
    };

    /**
     * @brief Verifier for DOSBox installations.
     */
    class DosboxVerifier final : public InstallationVerifier {
        using InstallationVerifier::InstallationVerifier;

    public:
        /**
         * @brief Checks if the DOSBox installation is valid.
         *
         * @return True if valid, false otherwise.
         */
        bool isInstallationValid() override;
    };

} // namespace DosboxStagingReplacer

#endif // INSTALLATIONVERIFIER_H
