#include <iostream>
#include <string>

#include "DataExporter.h"
#include "FileBackupService.h"
#include "StatementParser.h"
#include "GoGService.h"
#include "helpers/finders/InstallationFinder.h"
#include "helpers/scanners/DirectoryScanner.h"
#include "helpers/verifiers/InstallationVerifier.h"

#include "libs/argparse/argparse.hpp"


int main(int argc, char *argv[]) {

    DosboxStagingReplacer::FileBackupService fileBackupService;
    DosboxStagingReplacer::DataExporter dataExporter;

    argparse::ArgumentParser program("Dosbox Staging Replacer");
    program.add_argument("-f", "--file")
        .help("The Galaxy database file")
        .default_value(std::string("galaxy-2.0.db"))
        .nargs(1);
    program.add_argument("-d", "--directory")
        .help("The GoG Galaxy storage directory")
        .default_value(std::string(R"(C:\ProgramData\GOG.com\Galaxy\storage)"))
        .nargs(1);
    program.add_argument("-b", "--backup")
        .help("Create a backup of the Galaxy database")
        .default_value(false)
        .implicit_value(true)
        .nargs(0);
    program.add_argument("-r", "--restore")
        .help("Restore the backup of the Galaxy database")
        .default_value(false)
        .implicit_value(true)
        .nargs(0);
    program.add_argument("-la", "--list-applications")
        .help("Print all installed applications")
        .default_value(false)
        .implicit_value(true)
        .nargs(0);
    program.add_argument("-lg", "--list-games")
        .help("Print all installed games")
        .default_value(false)
        .implicit_value(true)
        .nargs(0);
    program.add_argument("-fmt", "--format")
        .help("The format used when doing any print operation")
        .default_value(std::string(".json"))
        .choices(".json", ".csv", ".txt")
        .nargs(1);
    program.add_argument("-o", "--output")
        .help("The name of the output file. If \"\", the output will be printed to the console.")
        .default_value(std::string(""))
        .nargs(1);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // TODO: Add flag checking for what flags can be used together or each other
    // TODO: The checking must start below

    const auto chosenFile = program.get<std::string>("--file");
    const auto chosenPath = program.get<std::string>("--directory");
    std::vector<DosboxStagingReplacer::FileEntity> files;
    DosboxStagingReplacer::GogGalaxyService service;

    // auto applications = DosboxStagingReplacer::getInstalledApplications();
    // for (const auto &application : applications) {
    //     std::cout << application.applicationName << " (" << application.installationPath << ")" << std::endl;
    // }

    try {
        DosboxStagingReplacer::DirectoryScanner scanner;
        files = scanner.scanDirectory(chosenPath);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    if (program["--backup"] == true) {
        service.openConnection(chosenPath + "\\" + chosenFile);
        if (!service.isDatabaseValid()) {
            std::cerr << "Error: The database is not valid. Quitting program" << std::endl;
            return -1;
        }
        std::cout << "Creating a backup of " << chosenFile << " in " << chosenPath << std::endl;
        service.closeConnection();
        fileBackupService.createBackup(chosenPath + "\\" + chosenFile, files);
    }
    else if (program["--restore"] == true) {
        service.openConnection(chosenPath + "\\" + chosenFile);
        if (!service.isDatabaseValid()) {
            std::cerr << "Error: The database is not valid. Quitting program" << std::endl;
            return -1;
        }
        std::cout << "Restoring the backup of " << chosenFile << " in " << chosenPath << std::endl;
        service.closeConnection();
        fileBackupService.restoreFromBackup(chosenPath + "\\" + chosenFile, files);
    }
    else if (program["--list-applications"] == true) {
        auto applications = DosboxStagingReplacer::getInstalledApplications();
        std::cout << dataExporter.serialize(applications) << std::endl;
    }
    else if (program["--list-games"] == true) {
        // Need to convert the result of getProducts to a vector of shared pointers so we can
        // take advantage of polymorphism
        std::vector<std::shared_ptr<DosboxStagingReplacer::SqlDataResult>> games;
        service.openConnection(chosenPath + "\\" + chosenFile);
        for (auto& product : service.getProducts()) {
            games.push_back(std::make_shared<DosboxStagingReplacer::ProductDetails>(product));
        }
        std::cout << dataExporter.serialize(games) << std::endl;
        service.closeConnection();
    }

    return 0;
}
