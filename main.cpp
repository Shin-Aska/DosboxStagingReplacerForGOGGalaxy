#include <filesystem>
#include <iostream>
#include <string>
#include <clocale>

#ifdef _WIN32
#include <windows.h>
#endif

#include "DataExporter.h"
#include "FileBackupService.h"
#include "GogGalaxyService.h"
#include "ScriptEditService.h"
#include "helpers/finders/InstallationFinder.h"
#include "helpers/scanners/DirectoryScanner.h"
#include "helpers/verifiers/InstallationVerifier.h"

#include "libs/argparse/argparse.hpp"

#define APP_VERSION "1.1.6"


int main(int argc, char *argv[]) {
#ifdef _WIN32
    // Set the console output code page to UTF-8
    // Exclusively for Windows as other platforms rely on the locale
    SetConsoleOutputCP(CP_UTF8);
#endif
    // Set the locale to UTF-8
    setlocale(LC_ALL, "en_US.utf8");

    // Parse command line arguments using argparse
    argparse::ArgumentParser program("Dosbox Staging Replacer", APP_VERSION);
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
            {"dosbox-staging", "DOSBox Staging"}, {"dosbox-x", "DOSBox-X"}, {"dosbox-pure", "DOSBoxPure"}};
    program.add_argument("-dv", "--dosbox-version")
            .help("The version of DOSBox to use. This is a string, the format is the application name that is "
                  "printed by the --list-applications flag. When using this flag, the version of DOSBox you want to "
                  "use must be installed in the system.")
            .default_value(std::string(""))
            .choices("", "dosbox-staging", "dosbox-x", "dosbox-pure")
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
    program.add_argument("-cfsdf", "--change-fullscreen-default")
            .help("Replaces the predefined value of the fullscreen value in the Dosbox config file to the Dosbox's defaults")
            .default_value(std::string("yes"))
            .choices("yes", "no", "true", "false")
            .nargs(1);
    program.add_argument("-crsdf", "--change-resolution-default")
            .help("Replaces the predefined value of the resolutions(fullresolution, windowresolution) value in the Dosbox config file to the Dosbox's defaults")
            .default_value(std::string("yes"))
            .choices("yes", "no", "true", "false")
            .nargs(1);
    program.add_argument("-cmpdf", "--change-mapping-default")
            .help("Removes the mapping value in the Dosbox config file")
            .default_value(std::string("yes"))
            .choices("yes", "no", "true", "false")
            .nargs(1);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    auto is_enabled = [&](const char* option) {
        const auto value = program.get<std::string>(option);
        return value != "no" && value != "false";
    };

    const bool replaceFullscreenDefaults = is_enabled("--change-fullscreen-default");
    const bool replaceResolutionDefaults = is_enabled("--change-resolution-default");
    const bool removeMappingDefaults     = is_enabled("--change-mapping-default");

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

    const auto operationsCount = std::ranges::count(flags, true);

    // If more than one flag is set to true, we print an error message and exit
    if (operationsCount > 1) {
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
    if (operationsCount == 1) {
        DosboxStagingReplacer::FileBackupService fileBackupService;
        const auto searchString = program.get<std::string>("--search");
        const std::filesystem::path chosenFile = program.get<std::string>("--file");
        const std::filesystem::path chosenPath = program.get<std::string>("--directory");
        const auto dataExporter =
                DosboxStagingReplacer::DataExporterFactory::createDataExporter(program.get<std::string>("--format"));
        std::vector<DosboxStagingReplacer::FileEntity> files;
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
            if (!searchString.empty()) {
                std::vector<DosboxStagingReplacer::InstallationInfo> filteredApplications;
                std::ranges::copy_if(applications, std::back_inserter(filteredApplications), [&](const auto &app) {
                    std::string lowerCaseName = app.applicationName;
                    std::ranges::transform(lowerCaseName, lowerCaseName.begin(), tolower);
                    std::string lowerCaseSearchString = searchString;
                    std::ranges::transform(lowerCaseSearchString, lowerCaseSearchString.begin(), tolower);
                    return lowerCaseName.find(lowerCaseSearchString) != std::string::npos;
                });
                applications = filteredApplications;
            }
            std::cout << dataExporter->serialize(applications) << std::endl;
        } else if (program["--list-games"] == true) {
            std::vector<DosboxStagingReplacer::ProductDetails> games;
            service.openConnection((chosenPath / chosenFile).string());
            for (auto &product: service.getProducts({}, program.get<bool>("--dos-only"))) {
                games.push_back(product);
            }
            if (!searchString.empty()) {
                std::vector<DosboxStagingReplacer::ProductDetails> filteredGames;
                std::ranges::copy_if(games, std::back_inserter(filteredGames), [&](const auto &game) {
                    std::string lowerCaseTitle = game.title;
                    std::ranges::transform(lowerCaseTitle, lowerCaseTitle.begin(), tolower);
                    std::string lowerCaseSearchString = searchString;
                    std::ranges::transform(lowerCaseSearchString, lowerCaseSearchString.begin(), tolower);
                    return lowerCaseTitle.find(lowerCaseSearchString) != std::string::npos;
                });
                games = filteredGames;
            }
            std::cout << dataExporter->serialize(games) << std::endl;
            service.closeConnection();
        } else if (program["--show-playtasks"] == true) {
            std::vector<DosboxStagingReplacer::PlayTaskInformation> playTasks;
            service.openConnection((chosenPath / chosenFile).string());
            for (auto &playTask: service.getPlayTasksFromGameReleaseKey(releaseKey)) {
                playTasks.push_back(playTask);
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
                std::cout << "Scanning chosen dosbox directory" << std::endl;
                auto dosBoxFiles =
                        DosboxStagingReplacer::DirectoryScanner::scanDirectory(application.front().installationPath);
                if (dosBoxFiles.empty()) {
                    std::cerr << "Error: There are no files in the application installation path" << std::endl;
                }

                std::cout << "Searching for dosbox.exe in the application installation path" << std::endl;

                auto dosBoxExeSearch = std::ranges::find_if(dosBoxFiles, [&](const auto &file) {
                    std::string lowerCaseName = file.name;
                    std::ranges::transform(lowerCaseName, lowerCaseName.begin(), tolower);
                    return lowerCaseName.find("dosbox") != std::string::npos &&
                           lowerCaseName.ends_with(".exe");
                });
                if (dosBoxExeSearch == dosBoxFiles.end()) {
                    std::cerr << "Error: There is no DOSBox application in the application installation path" << std::endl;
                    return -1;
                }
                dosBoxExe = std::make_shared<DosboxStagingReplacer::FileEntity>(*dosBoxExeSearch);
                std::cout << "Successfully found DOSBox application " << dosBoxExe->name << " in the application installation path" << std::endl;
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

            auto customTaskType =
                    std::ranges::find_if(taskTypes, [&](const auto &taskType) { return taskType.type == "Custom"; });
            if (customTaskType == taskTypes.end()) {
                service.addTaskType("Custom");
                std::cout << "Custom task type not found, adding it to the database" << std::endl;
                taskTypes = service.getPlayTaskTypes();
                customTaskType = std::ranges::find_if(taskTypes, [&](const auto &taskType) { return taskType.type == "Custom"; });
                if (customTaskType == taskTypes.end()) {
                    std::cerr << "Error: Could not find the custom task type after adding it to the database" << std::endl;
                    return -1;
                }
            }

            auto playTasks = service.getPlayTasksFromGameReleaseKey(releaseKey);
            auto primaryPlayTask = std::ranges::find_if(
                    playTasks, [&](const auto &playTask) { return playTask.type == "BuiltInPrimary"; });
            if (primaryPlayTask == playTasks.end()) {
                std::cerr << "Error: There are no primary play tasks for the game" << std::endl;
                return -1;
            }

            auto playTaskForInsertion = *primaryPlayTask;
            playTaskForInsertion.typeId = customTaskType->id;
            playTaskForInsertion.type = customTaskType->type;

            auto users = service.getUsers();
            if (users.empty()) {
                std::cerr << "Error: There are no users in the database" << std::endl;
            }

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

            service.setCustomLaunchParametersForProduct(releaseKey, true);

            std::cout << "Modifications completed" << std::endl;
            service.closeConnection();

            std::cout << "Modifying Dosbox configuration files for product" << std::endl;

            auto productFiles = DosboxStagingReplacer::DirectoryScanner::scanDirectory(product.installationPath);

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
                if (replaceFullscreenDefaults) {
                    std::cout << "Replacing fullscreen=true with fullscreen=false" << std::endl;
                    DosboxStagingReplacer::ScriptEditService::disableFullScreenForDosboxConfig(dosboxConfigPath);
                }
                if (replaceResolutionDefaults) {
                    std::cout << "Replacing resolution values for fullscreen and windowed display modes to defaults" << std::endl;
                    DosboxStagingReplacer::ScriptEditService::replaceDisplayToDefaultForDosboxConfig(dosboxConfigPath);
                }
                if (removeMappingDefaults) {
                    std::cout << "Removing overwritten mapping values and resetting it to defaults" << std::endl;
                    DosboxStagingReplacer::ScriptEditService::disableOverwrittenMappingForDosboxConfig(dosboxConfigPath);
                }
            }
            std::cout << "Successfully modified config files" << std::endl;
            std::cout << "Modifications complete! You may need to restart Gog galaxy to see the changes" << std::endl;
        }
    }

    return 0;
}
