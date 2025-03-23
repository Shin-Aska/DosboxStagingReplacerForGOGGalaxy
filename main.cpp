#include <iostream>
#include <vector>

#include "helpers/verifiers/DosboxVerifier.cpp"
#include "helpers/finders/InstallationFinder.cpp"
#include "helpers/scanners/DirectoryScanner.cpp"

using namespace DosboxStagingReplacer;

// TIP: The executable follows the launching format
// The following parameters are
// -t <targetExecutable> -p <installationPath>

int main(int argc, char *argv[]) {

    std::string path = "C:\\Development\\DemoFunctions";
    std::string targetExecutable = "Demo2.java";

    std::vector<std::string> args(argv, argv + argc);

    auto installationFinder = InstallationFinder();
    auto verifier = DosboxVerifier(path, targetExecutable);
    auto scanner = DirectoryScanner();

    // auto installedApps = getInstalledApplications();
    // for (auto &app : installedApps) {
    //     std::cout << app.applicationName << "\t(" << app.installationPath << ")" << std::endl;
    // }

    // auto files = DirectoryScanner();
    // auto scannedFiles = files.scanDirectory(path);
    // for (auto &file : scannedFiles) {
    //     std::cout << file.name << "\t(" << file.path << ")" << "\t" << file.size << "\t" << static_cast<int>(file.type) << std::endl;
    // }

    return 0;
}

