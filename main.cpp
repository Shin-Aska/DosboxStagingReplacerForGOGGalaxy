#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DataExporter.h"
#include "FileBackupService.h"
#include "GogGalaxyService.h"
#include "ScriptEditService.h"
#include "StatementParser.h"
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

    // Parse command line arguments using argparse
    argparse::ArgumentParser program("Dosbox Staging Replacer", "1.0.2");
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
    program.add_argument("-lb", "--list-backups")
            .help("List all backups of the Galaxy database")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-la", "--list-applications")
            .help("Print all installed applications")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-do", "--dos-only")
            .help("This flag signifies that any list operations will only be DOS related.")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-lg", "--list-games")
            .help("Print all installed games")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-sp", "--show-playtasks")
            .help("Shows all play task registered for a game")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-rd", "--replace-dosbox")
            .help("Replace the DOSBox version used by the game with the one you specified")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-alu", "--all-users")
            .help("This flag signifies that the operation will be performed for all users. "
                  "Otherwise, it will get the most recent User.")
            .default_value(false)
            .implicit_value(true)
            .nargs(0);
    program.add_argument("-s", "--search")
            .help("Filter the list results when listing applications or games. ")
            .default_value(std::string(""))
            .nargs(1);
    std::unordered_map<std::string, std::string> dosBoxVersionParameters = {
            {"dosbox-staging", "DOSBox Staging"}, {"dosbox-x", "DOSBox X"}, {"dosbox-ece", "DOSBox ECE"}};
    program.add_argument("-dv", "--dosbox-version")
            .help("The version of DOSBox to use. This is a string, the format is the application name that is "
                  "printed by the --list-applications flag. When using this flag, the version of DOSBox you want to "
                  "use must be installed in the system.")
            .default_value(std::string(""))
            .choices("", "dosbox-staging", "dosbox-x", "dosbox-ece")
            .nargs(1);

    program.add_argument("-dvm", "--dosbox-version-manual")
            .help("The version of DOSBox to use. This is a string containing the absolute path to the DOSBox "
                  "executable. "
                  "Use this if you want to use a version of DOSBox that is not installed in the system or is a "
                  "portable executable.")
            .default_value(std::string(""))
            .nargs(1);
    program.add_argument("-rk", "--release-key")
            .help("The release key of the selected game. You can get this from the --list-games flag. ")
            .default_value(std::string(""))
            .nargs(1);
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
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // Let us add flags checking where flags such as backup, restore, list-applications, list-games, list-backups
    //  and replace-dosbox cannot be used together. We declare a vector of bools to check, with the set of flags
    //  we want to check
    std::vector flags = {program["--backup"] == true,
                         program["--restore"] == true,
                         program["--list-applications"] == true,
                         program["--list-games"] == true,
                         program["--list-backups"] == true,
                         program["--replace-dosbox"] == true,
                         program["--show-playtasks"] == true};

    const auto operations_count = std::ranges::count(flags, true);

    // If more than one flag is set to true, we print an error message and exit
    if (operations_count > 1) {
        std::cerr << "Error: You can only use one of the following flags at a time: --backup, --restore, "
                     "--list-applications, --list-games, --list-backups, --replace-dosbox, --show-playtasks"
                  << std::endl;
        return -1;
    }

    // If the replace-dosbox flag is set to true, we check if there are values supplied for either dosbox-version or
    // dosbox-version-manual. There are following conditions that must be met:
    // 1. The replace-dosbox flag must be set to true and if so there must be a value for either dosbox-version or
    // dosbox-version-manual
    // 2. If there is value on both dosbox-version and dosbox-version-manual, we print an error message and exit
    std::vector replaceDosboxFlags = {!program.get<std::string>("--dosbox-version").empty(),
                                      !program.get<std::string>("--dosbox-version-manual").empty()};

    // If the show playtask flag is set to true, we check if the --release-key flag has a value provided, otherwise
    // we print an error message and exit

    const auto releaseKey = program.get<std::string>("--release-key");
    if (program["--show-playtasks"] == true) {
        if (releaseKey.empty()) {
            std::cerr << "Error: You must supply a value for --release-key" << std::endl;
            return -1;
        }
    }

    if (program["--replace-dosbox"] == true) {
        if (const auto count = std::ranges::count(replaceDosboxFlags, true); count > 1) {
            std::cerr << "Error: You can only use one of the following flags at a time: --dosbox-version, "
                         "--dosbox-version-manual when using --replace-dosbox"
                      << std::endl;
            return -1;
        } else if (count == 0) {
            std::cerr << "Error: You must supply a value for either --dosbox-version or --dosbox-version-manual when "
                         "using --replace-dosbox"
                      << std::endl;
            return -1;
        } else if (releaseKey.empty()) {
            std::cerr << "Error: You must supply a value for --release-key when using --replace-dosbox" << std::endl;
            return -1;
        }
    }

    // This is post parameter checking, we check if the --replace-dosbox flag is set to true and if so we check if
    // 1. If --dosbox-version is set, we check if the application is installed in the system, if not we print an error
    // message
    // 2. If --dosbox-version-manual is set, we check if the path is valid, if not we print an error message
    if (program["--replace-dosbox"] == true) {
        if (const auto dosboxArgument = program.get<std::string>("--dosbox-version"); !dosboxArgument.empty()) {
            if (auto applications = DosboxStagingReplacer::InstallationFinder::findApplication(
                        dosBoxVersionParameters[dosboxArgument]);
                applications.empty()) {
                std::cerr << "Error: The application you specified is not installed in the system" << std::endl;
                return -1;
            }
        } else if (const auto dosboxPath = program.get<std::string>("--dosbox-version-manual"); !dosboxPath.empty()) {
            if (!DosboxStagingReplacer::fileExists(dosboxPath)) {
                std::cerr << "Error: The path you specified does not exist" << std::endl;
                return -1;
            }
        }
    }

    // If there are no operation flags set, we do not do anything but print help
    if (operations_count == 1) {
        DosboxStagingReplacer::FileBackupService fileBackupService;
        // Search string for the --search flag
        const auto searchString = program.get<std::string>("--search");
        // We set the values from --file and --directory to variables
        const std::filesystem::path chosenFile = program.get<std::string>("--file");
        const std::filesystem::path chosenPath = program.get<std::string>("--directory");
        // Initialize a data exporter base on the format chosen by the user
        const auto dataExporter =
                DosboxStagingReplacer::DataExporterFactory::createDataExporter(program.get<std::string>("--format"));
        // We initialize a vector of FileEntity objects to store the files and use it later with DirectoryScanner
        // Then keep it so we can pass it to the file backup service, this skips the FileBackupService from re-scanning
        // the directory
        std::vector<DosboxStagingReplacer::FileEntity> files;
        // The service class for the GoG Galaxy database
        DosboxStagingReplacer::GogGalaxyService service;

        try {
            files = DosboxStagingReplacer::DirectoryScanner::scanDirectory(chosenPath.string());
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return -1;
        }

        if (program["--backup"] == true) {
            service.openConnection((chosenPath / chosenFile).string());
            if (!service.isDatabaseValid()) {
                std::cerr << "Error: The database is not valid. Quitting program" << std::endl;
                return -1;
            }
            std::cout << "Creating a backup of " << chosenFile << " in " << chosenPath << std::endl;
            service.closeConnection();
            auto backupFile = fileBackupService.createBackup((chosenPath / chosenFile).string(), files);
        } else if (program["--restore"] == true) {
            service.openConnection((chosenPath / chosenFile).string());
            if (!service.isDatabaseValid()) {
                std::cerr << "Error: The database is not valid. Quitting program" << std::endl;
                return -1;
            }
            std::cout << "Restoring the backup of " << chosenFile << " in " << chosenPath << std::endl;
            service.closeConnection();
            auto restoredFile = fileBackupService.restoreFromBackup((chosenPath / chosenFile).string(), files);
        } else if (program["--list-backups"] == true) {
            std::vector<DosboxStagingReplacer::FileEntity> filteredFiles;
            // We iterate to the files variable we already made and filter out non GogGalaxy database files
            std::ranges::copy_if(files, std::back_inserter(filteredFiles), [&](const auto &file) {
                return file.path.find(chosenFile.string() + fileBackupService.getBackupFileExtension()) !=
                       std::string::npos;
            });
            std::cout << dataExporter->serialize(filteredFiles) << std::endl;
        } else if (program["--list-applications"] == true) {
            std::vector<DosboxStagingReplacer::InstallationInfo> applications;
            if (program["--dos-only"] == true)
                applications = DosboxStagingReplacer::InstallationFinder::findApplication("DOSBox");
            else
                applications = DosboxStagingReplacer::getInstalledApplications();
            // Filter the applications based on the search string if it is not empty
            if (!searchString.empty()) {
                std::vector<DosboxStagingReplacer::InstallationInfo> filteredApplications;
                std::ranges::copy_if(applications, std::back_inserter(filteredApplications), [&](const auto &app) {
                    return app.applicationName.find(searchString) != std::string::npos;
                });
                applications = filteredApplications;
            }
            std::cout << dataExporter->serialize(applications) << std::endl;
        } else if (program["--list-games"] == true) {
            // Need to convert the result of getProducts to a vector of shared pointers so we can
            // take advantage of polymorphism
            std::vector<std::shared_ptr<DosboxStagingReplacer::SqlDataResult>> games;
            service.openConnection((chosenPath / chosenFile).string());
            for (auto &product: service.getProducts({}, program.get<bool>("--dos-only"))) {
                games.push_back(std::make_shared<DosboxStagingReplacer::ProductDetails>(product));
            }
            // Filter the applications based on the search string if it is not empty
            if (!searchString.empty()) {
                std::vector<std::shared_ptr<DosboxStagingReplacer::SqlDataResult>> filteredGames;
                std::ranges::copy_if(games, std::back_inserter(filteredGames),
                                     [&](const std::shared_ptr<DosboxStagingReplacer::SqlDataResult> &game) {
                                         // We need to cast the SqlDataResult to ProductDetails to access the title
                                         const auto *gameDetails =
                                                 dynamic_cast<DosboxStagingReplacer::ProductDetails *>(game.get());
                                         return game && gameDetails->title.find(searchString) != std::string::npos;
                                     });
                games = filteredGames;
            }
            std::cout << dataExporter->serialize(games) << std::endl;
            service.closeConnection();
        } else if (program["--show-playtasks"] == true) {
            std::vector<std::shared_ptr<DosboxStagingReplacer::SqlDataResult>> playTasks;
            service.openConnection((chosenPath / chosenFile).string());
            for (auto &playTask: service.getPlayTasksFromGameReleaseKey(releaseKey)) {
                playTasks.push_back(std::make_shared<DosboxStagingReplacer::PlayTaskInformation>(playTask));
            }
            std::cout << dataExporter->serialize(playTasks) << std::endl;
            service.closeConnection();
        } else if (program["--replace-dosbox"] == true) {
            const auto dosboxArgument = program.get<std::string>("--dosbox-version");
            const auto dosboxManualPath = program.get<std::string>("--dosbox-version-manual");
            std::shared_ptr<DosboxStagingReplacer::FileEntity> dosBoxExe;

            if (!dosboxArgument.empty()) {
                auto application = DosboxStagingReplacer::InstallationFinder::findApplication(
                        dosBoxVersionParameters[dosboxArgument]);
                // No need to do checking here and should not be done because we already did that at the earlier parts
                // of the code Scan the files in the application installation path
                std::cout << "Scanning chosen dosbox directory" << std::endl;
                auto dosBoxFiles =
                        DosboxStagingReplacer::DirectoryScanner::scanDirectory(application.front().installationPath);
                // If somehow there are no files under that folder, we return an error
                if (dosBoxFiles.empty()) {
                    std::cerr << "Error: There are no files in the application installation path" << std::endl;
                }

                std::cout << "Searching for dosbox.exe in the application installation path" << std::endl;

                // We search if there is dosbox.exe in the directory, we search in non-case sensitive search
                auto dosBoxExeSearch = std::ranges::find_if(dosBoxFiles, [&](const auto &file) {
                    std::string lowerCaseName = file.name;
                    std::ranges::transform(lowerCaseName, lowerCaseName.begin(), tolower);
                    return lowerCaseName == "dosbox.exe";
                });
                // We search if there is dosbox.exe, if there is none, then we return an error
                if (dosBoxExeSearch == dosBoxFiles.end()) {
                    std::cerr << "Error: There is no dosbox.exe in the application installation path" << std::endl;
                    return -1;
                }
                // Assign dosBoxExeSearch to dosBoxExe
                dosBoxExe = std::make_shared<DosboxStagingReplacer::FileEntity>(*dosBoxExeSearch);
                std::cout << "Successfully found dosbox.exe in the application installation path" << std::endl;
            } else if (!dosboxManualPath.empty()) {
                std::filesystem::path manualPath = dosboxManualPath;
                DosboxStagingReplacer::FileEntity fileEntity;
                fileEntity.name = manualPath.filename().string();
                fileEntity.path = manualPath.string();
                dosBoxExe = std::make_shared<DosboxStagingReplacer::FileEntity>(std::move(fileEntity));
                std::cout << "Successfully parsed --dosbox-manual-version parameter value for finding dosbox.exe"
                          << std::endl;
            }

            service.openConnection((chosenPath / chosenFile).string());

            std::cout << "Getting the product information for the provided --release-key" << std::endl;

            auto products = service.getProducts(releaseKey, program.get<bool>("--dos-only"));
            if (products.empty()) {
                std::cerr << "Error: There are no products with the release key " << releaseKey << std::endl;
                return -1;
            }

            const auto& product = products.front();
            std::filesystem::path productPath = product.installationPath;
            auto taskTypes = service.getPlayTaskTypes();

            // Find the Custom Task Type by finding the task type that has the type Custom
            auto customTaskType =
                    std::ranges::find_if(taskTypes, [&](const auto &taskType) { return taskType.type == "Custom"; });
            if (customTaskType == taskTypes.end()) {
                std::cerr << "Error: There are no custom task types registered in the database" << std::endl;
                return -1;
            }

            auto playTasks = service.getPlayTasksFromGameReleaseKey(releaseKey);
            // Get the primary playTask for that game by finding the one which has the type value BuiltInPrimary
            auto primaryPlayTask = std::ranges::find_if(
                    playTasks, [&](const auto &playTask) { return playTask.type == "BuiltInPrimary"; });
            if (primaryPlayTask == playTasks.end()) {
                std::cerr << "Error: There are no primary play tasks for the game" << std::endl;
                return -1;
            }

            // Create a clone of primary playTask for modification
            auto playTaskForInsertion = *primaryPlayTask;
            playTaskForInsertion.typeId = customTaskType->id;
            playTaskForInsertion.type = customTaskType->type;

            auto users = service.getUsers();
            if (users.empty()) {
                std::cerr << "Error: There are no users in the database" << std::endl;
            }

            // Get Launch Parameters from the Primary Play Task
            auto launchParameters = service.getPlayTaskLaunchParametersFromPlayTaskId(primaryPlayTask->id);
            if (launchParameters.empty()) {
                std::cerr << "Error: There are no launch parameters for the primary play task" << std::endl;
            }
            auto launchParametersForInsertion = launchParameters.front();
            launchParametersForInsertion.executablePath = dosBoxExe->path;
            launchParametersForInsertion.commandLineArgs =
                    DosboxStagingReplacer::ScriptEditService::resolveRelativePathsFromString(
                            launchParametersForInsertion.commandLineArgs, productPath);

            std::cout << "Product information successfully retrieved" << std::endl;
            std::cout << "Adding changes to the Gog database" << std::endl;

            // If --all-users is set, we iterate all users, otherwise we jump to the last part of
            // the vector

            // Now we do the real work, add play task here on Gog database
            if (program.get<bool>("--all-users") == true) {
                for (const auto& user: users) {
                    service.addPlayTask(user.id, releaseKey, playTaskForInsertion, launchParametersForInsertion);
                }
                std::cout << "Successfully added play task for all users" << std::endl;
            } else {
                const auto& user = users.back();
                service.addPlayTask(user.id, releaseKey, playTaskForInsertion, launchParametersForInsertion);
                std::cout << "Successfully added play task for most recent user" << std::endl;
            }

            // Afterward we set the custom launch parameters to enable for this
            service.setCustomLaunchParametersForProduct(releaseKey, true);

            std::cout << "Modifications completed" << std::endl;

            // We are now done with adjusting anything on Gog!
            service.closeConnection();

            std::cout << "Modifying Dosbox configuration files for product" << std::endl;

            // We finally adjust the files using ScriptEditService
            auto productFiles = DosboxStagingReplacer::DirectoryScanner::scanDirectory(product.installationPath);
            // Find the config files, all of these files contain [autoexec] in their file

            std::vector<DosboxStagingReplacer::FileEntity> configAutoExecFiles;
            std::ranges::copy_if(productFiles, std::back_inserter(configAutoExecFiles), [&](const auto &file) {
                const std::filesystem::path filePath = file.path;
                return DosboxStagingReplacer::ScriptEditService::isConfigFileDosboxAutoExec(filePath);
            });

            std::cout << "Found " << configAutoExecFiles.size() << " config files to modify" << std::endl;

            for (const auto &configAutoExec: configAutoExecFiles) {
                std::filesystem::path configAutoExecPath = configAutoExec.path;
                std::cout << "Modifying " << configAutoExec.path << "..." << std::endl;
                DosboxStagingReplacer::ScriptEditService::resolveRelativePathsForDosboxAutoExec(configAutoExecPath,
                                                                                                productPath);
            }

            std::cout << "Successfully modified autoexec files for product" << std::endl;
            std::cout << "Modifying config files (disabling fullscreen=false)" << std::endl;
            std::vector<DosboxStagingReplacer::FileEntity> dosboxConfigFiles;
            std::ranges::copy_if(productFiles, std::back_inserter(dosboxConfigFiles), [&](const auto &file) {
                const std::filesystem::path filePath = file.path;
                return DosboxStagingReplacer::ScriptEditService::isConfigFileDosboxConfig(filePath);
            });
            for (const auto &dosboxConfig: dosboxConfigFiles) {
                std::filesystem::path dosboxConfigPath = dosboxConfig.path;
                std::cout << "Modifying " << dosboxConfig.path << "..." << std::endl;
                DosboxStagingReplacer::ScriptEditService::disableFullScreenForDosboxConfig(dosboxConfigPath);
            }
            std::cout << "Successfully modified config files" << std::endl;
            std::cout << "Modifications complete! You may need to restart Gog galaxy to see the changes" << std::endl;
        }
    }

    return 0;
}
