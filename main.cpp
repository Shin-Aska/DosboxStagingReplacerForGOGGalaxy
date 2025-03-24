#include <iostream>
#include <vector>

#include "services/gog/Service.cpp"

using namespace DosboxStagingReplacer;

// TIP: The executable follows the launching format
// The following parameters are
// -t <targetExecutable> -p <installationPath>

int main(int argc, char *argv[]) {

    auto service = GogGalaxyService("galaxy-2.1.db");
    std::cout << "Database is valid: " << service.isDatabaseValid() << std::endl;

    return 0;
}
