#include <StatementParser.h>
#include <iostream>
#include "GogGalaxyService.h"

int main() {
    std::cout << "Testing GogGalaxyService::openConnection() with valid GOG Galaxy database" << std::endl;
    DosboxStagingReplacer::GogGalaxyService validService("../tests/data/valid.sqlite");
    DosboxStagingReplacer::GogGalaxyService invalidService("../tests/data/invalid.sqlite");

    std::cout << "Testing GogGalaxyService::verifyDatabase()" << std::endl;
    if (!validService.verifyDatabase()) {
        std::cout << "GogGalaxyService::verifyDatabase() failed" << std::endl;
        return 1;
    }
    std::cout << "GogGalaxyService::verifyDatabase() passed" << std::endl;

    std::cout << "Testing GogGalaxyService::verifyDatabase() with invalid GOG Galaxy database" << std::endl;
    if (invalidService.verifyDatabase()) {
        std::cout << "GogGalaxyService::verifyDatabase() should fail with invalid database" << std::endl;
        return 1;
    }
    std::cout << "GogGalaxyService::verifyDatabase() return false as expected."<< std::endl;
    std::cout << "Testing GogGalaxyService::openConnection() passed" << std::endl;

    std::cout << "Testing GogGalaxyService::getPlayTasks()" << std::endl;
    const auto tasks = validService.getPlayTasks();
    std::cout << "GogGalaxyService::getPlayTasks() returned " << tasks.size() << " tasks" << std::endl;

    std::cout << "Testing GogGalaxyService::getPlayTaskTypes()" << std::endl;
    const auto types = validService.getPlayTaskTypes();
    std::cout << "GogGalaxyService::getPlayTaskTypes() returned " << types.size() << " types" << std::endl;

    std::cout << "Testing GogGalaxyService attempting to getPlayTasks from invalid database" << std::endl;
    try {
        const auto tasks2 = invalidService.getPlayTasks();
        std::cout << "GogGalaxyService::getPlayTasks() should fail with invalid database" << std::endl;
        return 1;
    } catch (const DosboxStagingReplacer::GogGalaxyServiceException &e) {
        std::cout << "GogGalaxyService::getPlayTasks() failed as expected: " << e.what() << std::endl;
    }

    std::cout << "Testing GogGalaxyService::getPlayTaskTypes() with invalid database" << std::endl;
    try {
        const auto types2 = invalidService.getPlayTaskTypes();
        std::cout << "GogGalaxyService::getPlayTaskTypes() should fail with invalid database" << std::endl;
        return 1;
    } catch (const DosboxStagingReplacer::GogGalaxyServiceException &e) {
        std::cout << "GogGalaxyService::getPlayTaskTypes() failed as expected: " << e.what() << std::endl;
    }

    return 0;
}
