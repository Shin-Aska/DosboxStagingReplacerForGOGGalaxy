#include <iostream>
#include <string>

#include "GoGService.h"
#include "helpers/finders/InstallationFinder.h"
#include "helpers/scanners/DirectoryScanner.h"
#include "helpers/verifiers/InstallationVerifier.h"


int main(int argc, char *argv[]) {

    DosboxStagingReplacer::DirectoryScanner scanner;
    DosboxStagingReplacer::GogGalaxyService service("galaxy-2.0.db");

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

    std::cout << "Is Database Valid? " << (service.isDatabaseValid() == true ? "Yes" : "No") << std::endl;

    // Get all products
    auto products = service.getProducts();
    for (const auto &product : products) {
        std::cout << product.gogId << " (" << product.releaseKey << ")" << std::endl;
    }
    return 0;
}
