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
        static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    public:
        /**
         * @brief Check if the given path is a DOSBox autoexec configuration file.
         * @param filePath path The absolute path to the file.
         * @return True if the file is a DOSBox autoexec configuration file, false otherwise.
         */
        static bool isConfigFileDosboxAutoExec(std::filesystem::path filePath);
        /**
         * @brief Resolve relative paths in the DOSBox autoexec configuration file.
         * @param filePath The path of the file to resolve relative paths in.
         * @param basePath The base path to resolve relative paths against.
         * @param tmpExtension The extension to use for the temporary file.
         */
        static void resolveRelativePathsForDosboxAutoExec(std::filesystem::path &filePath, const std::filesystem::path &basePath, const std::string &tmpExtension = ".tmp");
    };

} // DosboxStagingReplacer

#endif //SCRIPTEDITSERVICE_H
