#include <iostream>
#include <vector>

#include "services/gog/Service.cpp"

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

    return 0;
}
