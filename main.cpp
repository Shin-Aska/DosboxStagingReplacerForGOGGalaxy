#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DataExporter.h"
#include "FileBackupService.h"
#include "StatementParser.h"
#include "GoGService.h"
#include "helpers/finders/InstallationFinder.h"
#include "helpers/scanners/DirectoryScanner.h"
#include "helpers/verifiers/InstallationVerifier.h"

#include "libs/argparse/argparse.hpp"


int main(int argc, char *argv[]) {
#ifdef _WIN32
    // Set the console output code page to UTF-8
    // Exclusively for Windows as other platforms rely on the locale
    SetConsoleOutputCP(CP_UTF8);
#endif
    // Set the locale to UTF-8
    setlocale(LC_ALL, "en_US.utf8");

    // Initialize the file backup service
    DosboxStagingReplacer::FileBackupService fileBackupService;

    // Parse command line arguments using argparse
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

    // backup, restore, list-applications, list-games if two or more are used together, then we throw an error
    std::vector<bool> flags = {
        program["--backup"] == true,
        program["--restore"] == true,
        program["--list-applications"] == true,
        program["--list-games"] == true
    };
    if (std::ranges::count(flags, true) > 1) {
        std::cerr << "Error: You can only use one of the following flags at a time: --backup, --restore, --list-applications, --list-games" << std::endl;
        return -1;
    }

    // We set the values from --file and --directory to variables
    const auto chosenFile = program.get<std::string>("--file");
    const auto chosenPath = program.get<std::string>("--directory");
    // Initialize a data exporter base on the format chosen by the user
    const auto dataExporter = DosboxStagingReplacer::DataExporterFactory::createDataExporter(program.get<std::string>("--format"));
    // We initialize a vector of FileEntity objects to store the files and use it later with DirectoryScanner
    // Then keep it so we can pass it to the file backup service, this skips the FileBackupService from re-scanning the directory
    std::vector<DosboxStagingReplacer::FileEntity> files;
    // The service class for the GoG Galaxy database
    DosboxStagingReplacer::GogGalaxyService service;

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
        const auto applications = DosboxStagingReplacer::getInstalledApplications();
        std::cout << dataExporter->serialize(applications) << std::endl;
    }
    else if (program["--list-games"] == true) {
        // Need to convert the result of getProducts to a vector of shared pointers so we can
        // take advantage of polymorphism
        std::vector<std::shared_ptr<DosboxStagingReplacer::SqlDataResult>> games;
        service.openConnection(chosenPath + "\\" + chosenFile);
        for (auto& product : service.getProducts()) {
            games.push_back(std::make_shared<DosboxStagingReplacer::ProductDetails>(product));
        }
        std::cout << dataExporter->serialize(games) << std::endl;
        service.closeConnection();
    }

    return 0;
}
