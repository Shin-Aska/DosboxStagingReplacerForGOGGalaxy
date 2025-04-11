#ifndef FILEBACKUPSERVICE_H
#define FILEBACKUPSERVICE_H

#include <string>
#include <vector>
#include "CoreHelperModels.h"
#include "InstallationVerifier.h"

namespace DosboxStagingReplacer {

    class FileBackupService {
        private:
            std::string backupFileExtension = ".bak";
        public:
            FileBackupService() = default;

            // Function to set the backup file extension
            // @param extension The new backup file extension
            // @return void
            void setBackupFileExtension(const std::string& extension);
            // Function to get the backup file extension
            // @return The current backup file extension
            std::string getBackupFileExtension();

            // Function to create a backup of a file
            // @param filePath The path of the file to back up
            // @param filesInPath A vector of FileEntity objects representing the files in the path. If empty, then it will
            // try to get the files from the file system base on the filePath
            // @return void
            void createBackup(const std::string& filePath, const std::vector<FileEntity> &filesInPath = {}) const;

            // Function to restore a file from its backup
            // @param filePath The path of the file to restore
            // @return void
            void restoreFromBackup(const std::string &filePath, std::vector<FileEntity> filesInPath = {}) ;

            // Function to check if a backup exists for a file
            // @param filePath The path of the file to check
            // @param filesInPath A vector of FileEntity objects representing the files in the path. If empty, then it will
            // try to get the files from the file system base on the filePath
            // @return true if a backup exists, false otherwise
            static bool backupExists(const std::string& filePath, const std::vector<FileEntity> &filesInPath = {}) ;
    };

}

#endif //FILEBACKUPSERVICE_H
