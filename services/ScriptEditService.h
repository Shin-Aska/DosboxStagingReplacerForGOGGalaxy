//
// Created by Orill on 4/16/2025.
//

#ifndef SCRIPTEDITSERVICE_H
#define SCRIPTEDITSERVICE_H

#include <filesystem>

namespace DosboxStagingReplacer {
    /**
    * @brief A service for editing Scripts
    */
    class ScriptEditService {
    public:
        /**
         * @brief Check if the given path is a DOSBox autoexec configuration file.
         * @param filePath path The absolute path to the file.
         * @return True if the file is a DOSBox autoexec configuration file, false otherwise.
         */
        static bool isConfigFileDosboxAutoExec(const std::filesystem::path &filePath);
        /**
         * @brief Checks if the given file is a valid DOSBox configuration file.
         * @param filePath The absolute path to the file to be checked.
         * @return True if the file contains the required DOSBox configuration sections, false otherwise.
         */
        static bool isConfigFileDosboxConfig(const std::filesystem::path &filePath);


        /**
         * @brief Resolves relative paths in a command string using a specified base path.
         * @param cmd The command string that may contain relative paths.
         * @param basePath The base path used to resolve relative paths to absolute paths.
         * @return A new string with all relative paths in the command string resolved to absolute paths.
         */
        static std::string resolveRelativePathsFromString(const std::string &cmd,
                                                          const std::filesystem::path &basePath);
        /**
         * @brief Resolve relative paths in the DOSBox autoexec configuration file.
         * @param filePath The path of the file to resolve relative paths in.
         * @param basePath The base path to resolve relative paths against.
         * @param tmpExtension The extension to use for the temporary file.
         */
        static void resolveRelativePathsForDosboxAutoExec(std::filesystem::path &filePath, const std::filesystem::path &basePath, const std::string &tmpExtension = ".tmp");

        /**
         * @brief Disables fullscreen for the DOSBox configuration file.
         * @param filePath The path of the DOSBox configuration file to modify.
         * @param tmpExtension The extension to use for the temporary file
         */
        static void disableFullScreenForDosboxConfig(std::filesystem::path &filePath, const std::string &tmpExtension = ".tmp");

        /**
         * @brief Disables the mapping config for the DOSBox configuration file some games have.
         * @param filePath
         * @param tmpExtension
         */
        static void disableOverwrittenMappingForDosboxConfig(std::filesystem::path &filePath, const std::string &tmpExtension = ".tmp");

        /**
         * @brief Replaces the fullscreenresolution and windowresolution to the default's values
         * which is desktop and original respectively
         * @param filePath
         * @param tmpExtension
         */
        static void replaceDisplayToDefaultForDosboxConfig(std::filesystem::path &filePath, const std::string &tmpExtension = ".tmp");


        /**
         *
         * @param arg The current argument from Gog Galaxy
         * @param basePath The basePath string that we can use to replace relative path strings
         * @return A transformed config argument that has its relative path's replace with absolute path strings
         */
        static std::string transformConfArg(const std::string& arg, const std::filesystem::path &basePath);
    private:
        /**
         * @brief Replaces all occurrences of 'from' substring with 'to' substring in the given string.
         *
         * @param str The string in which replacements are to be made.
         * @param from The substring to be replaced.
         * @param to The substring to replace with.
         */
        static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    };

} // DosboxStagingReplacer

#endif //SCRIPTEDITSERVICE_H
