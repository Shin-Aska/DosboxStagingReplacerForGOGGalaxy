#include <iostream>
#include <string>
#include "scanners/DirectoryScanner.h"

int main(int argc, char *argv[]) {

    DosboxStagingReplacer::DirectoryScanner scanner;
    // Print the path to the directory
#ifdef __linux__
    std::string path = "/home/richard/Downloads";
#else
    std::string path = "C:\\Users\\richa\\Downloads";
#endif
    auto files = scanner.scanDirectory(path);
    for (const auto &file : files) {
        std::cout << file.name << " (" << file.path << ")" << std::endl;
    }
    return 0;
}
