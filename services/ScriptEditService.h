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
    private:
        static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    public:
        /**
         * @brief Check if the given path is a DOSBox autoexec configuration file.
         * @param path The absolute path to the file.
         * @return True if the file is a DOSBox autoexec configuration file, false otherwise.
         */
        static bool isConfigFileDosboxAutoExec(std::filesystem::path filePath);
        /**
         * @brief Resolve relative paths in the DOSBox autoexec configuration file.
         * @param path The path to the DOSBox autoexec configuration file.
         * @param basePath The base path to resolve relative paths against.
         */
        static void resolveRelativePathsForDosboxAutoExec(std::filesystem::path &filePath, const std::filesystem::path &basePath, const std::string &tmpExtension = ".tmp");
    };

} // DosboxStagingReplacer

#endif //SCRIPTEDITSERVICE_H
