#include <iostream>
#include <vector>

#include "services/gog/Service.cpp"
#include "helpers/finders/InstallationFinder.cpp"
#include "helpers/scanners/DirectoryScanner.cpp"

using namespace DosboxStagingReplacer;

// TIP: The executable follows the launching format
// The following parameters are
// -t <targetExecutable> -p <installationPath>

int main(int argc, char *argv[]) {

    auto service = GogGalaxyService("galaxy-2.0.db");
    std::cout << "Database is valid: " << service.isDatabaseValid() << std::endl;

    std::cout << "Products:" << std::endl;
    for (const auto &product : service.getProducts()) {
        std::cout << product.title << " (" << product.productId << ")" << std::endl;
    }

    std::cout << "Users:" << std::endl;
    for (const auto &user : service.getUsers()) {
        std::cout << user.id << std::endl;
    }

    std::cout << "Play Tasks:" << std::endl;
    for (const auto &task : service.getPlayTasks()) {
        std::cout << task.id << " (" << task.gameReleaseKey << ")" << std::endl;
    }

    std::cout << "Play Task Launch Parameters:" << std::endl;
    for (const auto &param : service.getPlayTaskLaunchParameters()) {
        std::cout << param.playTaskId << " (" << param.executablePath << ")" << std::endl;
    }

    // auto installedApplications = getInstalledApplications();
    // for (const auto &app : installedApplications) {
    //     std::cout << app.applicationName << " (" << app.installationPath << ")" << std::endl;
    // }
    //
    // auto scanner = DirectoryScanner();
    // auto files = scanner.scanDirectory("C:\\Development\\personal-website-repository\\articles");
    // for (const auto &file : files) {
    //     std::cout << file.name << " (" << file.path << ")" << std::endl;
    // }

    return 0;
}
