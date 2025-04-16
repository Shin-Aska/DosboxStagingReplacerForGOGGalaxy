#ifndef FILEBACKUPSERVICE_H
#define FILEBACKUPSERVICE_H

#include <string>
#include <vector>
#include "CoreHelperModels.h"
#include "InstallationVerifier.h"

namespace DosboxStagingReplacer {

    /**
     * @brief Service responsible for creating, restoring, and managing file backups.
     */
    class FileBackupService {
        std::string backupFileExtension = ".bak";

    public:
        /// @brief Default constructor
        FileBackupService() = default;

        /**
         * @brief Sets the file extension used for backups.
         * @param extension The new backup file extension.
         */
        void setBackupFileExtension(const std::string &extension);

        /**
         * @brief Retrieves the current backup file extension.
         * @return The current backup file extension as a string.
         */
        std::string getBackupFileExtension();

        /**
         * @brief Creates a backup of a file.
         * @param filePath The path of the file to back up.
         * @param filesInPath A vector of FileEntity objects representing the files in the path.
         * @return The FileEntity object representing the backup file.
         * If empty, the method will gather file information from the file system.
         */
        [[nodiscard]] FileEntity createBackup(const std::string &filePath, const std::vector<FileEntity> &filesInPath = {}) const;

        /**
         * @brief Restores a file from its backup.
         * @param filePath The path of the file to restore.
         * @param filesInPath A vector of FileEntity objects representing the files in the path.
         * @return The FileEntity object representing the restored file.
         */
        FileEntity restoreFromBackup(const std::string &filePath,
                                            const std::vector<FileEntity> &filesInPath = {});

        /**
         * @brief Checks whether a backup exists for a given file.
         * @param filePath The path of the file to check.
         * @param filesInPath A vector of FileEntity objects representing the files in the path.
         * If empty, the method will gather file information from the file system.
         * @return true if a backup exists, false otherwise.
         */
        bool backupExists(const std::string& filePath, const std::vector<FileEntity> &filesInPath = {});
    };

}

#endif // FILEBACKUPSERVICE_H
