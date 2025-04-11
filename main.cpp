#include <iostream>
#include <string>
#include "helpers/scanners/DirectoryScanner.h"
#include "helpers/finders/InstallationFinder.h"
#include "helpers/verifiers/InstallationVerifier.h"
#include "services/sql/Service.h"
#include "models/Data.h"


int main(int argc, char *argv[]) {

    DosboxStagingReplacer::DirectoryScanner scanner;
    DosboxStagingReplacer::SqlLiteService service("galaxy-2.0.db");

    // Environment specific variables
#ifdef __linux__
    std::string path = "/home/richard/Downloads";
    std::string existingFile = "/home/richard/Downloads/install-vs-code.bat";
    std::string nonExistingFile = "/home/richard/Downloads/superwave.wsz2";
#else
    std::string path = "C:\\Users\\Orill\\Downloads";
    std::string existingFile = "C:\\Users\\Orill\\Downloads\\instspeedfan452.exe";
    std::string nonExistingFile = "C:\\Users\\Orill\\Downloads\\superwave.wsz2";
#endif

    auto files = scanner.scanDirectory(path);
    for (const auto &file : files) {
        std::cout << file.name << " (" << file.path << ")" << std::endl;
    }

    auto applications = DosboxStagingReplacer::getInstalledApplications();
    for (const auto &application : applications) {
        std::cout << application.applicationName << " (" << application.installationPath << ")" << std::endl;
    }

    std::cout << existingFile << " Exists? " <<  (DosboxStagingReplacer::fileExists(existingFile) == true ? "Yes" : "No") << std::endl;
    std::cout << nonExistingFile << " Exists? " <<  (DosboxStagingReplacer::fileExists(nonExistingFile) == true ? "Yes" : "No") << std::endl;

    std::cout << "Is SqliteService connected? " << (service.isConnectionOpen() == true ? "Yes" : "No") << std::endl;

    auto result = service.executeQuery<DosboxStagingReplacer::SqliteSchema>(R"SQL(
        SELECT
            type,
            name,
            tbl_name,
            rootpage
        FROM sqlite_schema
        WHERE name IN
        ('Product Details View', 'ProductsToReleaseKeys', 'InstalledBaseProducts');
    )SQL", {});
    return 0;
}
