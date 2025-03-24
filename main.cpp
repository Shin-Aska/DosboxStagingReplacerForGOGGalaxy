#include <iostream>
#include <vector>

#include "helpers/finders/InstallationFinder.cpp"
#include "helpers/scanners/DirectoryScanner.cpp"
#include "helpers/verifiers/DosboxVerifier.cpp"
#include "services/sql/sqlite.cpp"
#include "models/results/SqliteSchema.cpp"
#include "models/results/ProductDetails.cpp"

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

    auto sqlService = SqlLiteService();
    sqlService.openConnection("galaxy-2.0.db");
    // auto result = sqlService.executeQuery<SqliteSchema>(""
    //                                       "SELECT "
    //                                       "type, name, tbl_name, rootpage "
    //                                       "FROM "
    //                                       "sqlite_schema "
    //                                       "WHERE "
    //                                       "type ='table' AND "
    //                                       "name NOT LIKE 'sqlite_%';",
    //                                       {}, true);
    //
    // for (auto &row : result) {
    //     std::cout << row.type << "\t" << row.name << "\t" << row.tbl_name << "\t" << row.rootpage << std::endl;
    // }

    auto result2 = sqlService.executeQuery<ProductDetails>(""
                                                          "SELECT "
                                                          "pdv.productId, pdv.title, pdv.slug, ptr.gogId, ptr.releaseKey "
                                                          "FROM "
                                                          "[Product Details Vie pdv "
                                                          "INNER JOIN ProductsToReleaseKeys ptr "
                                                          "ON ptr.gogId = pdv.productId",
                                                          {}, true);

    for (auto &row : result2) {
        std::cout << row.productId << "\t" << row.title << "\t" << row.slug << "\t" << row.gogId << "\t" << row.releaseKey << std::endl;
    }

    // auto installedApps = getInstalledApplications();
    // for (auto &app : installedApps) {
    //     std::cout << app.applicationName << "\t(" << app.installationPath << ")" << std::endl;
    // }

    // auto files = DirectoryScanner();
    // auto scannedFiles = files.scanDirectory(path);
    // for (auto &file : scannedFiles) {
    //     std::cout << file.name << "\t(" << file.path << ")" << "\t" << file.size << "\t" <<
    //     static_cast<int>(file.type) << std::endl;
    // }

    return 0;
}
