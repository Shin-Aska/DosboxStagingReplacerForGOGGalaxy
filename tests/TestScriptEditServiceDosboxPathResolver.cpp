#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ScriptEditService.h"

int main() {
    const std::filesystem::path inputPath = "../tests/data/input.dosbox.conf";
    const std::filesystem::path expectedPath = "../tests/data/valid.dosbox.conf";
    const std::filesystem::path tempPath = "../tests/data/input_tmp.dosbox.conf";

    std::error_code ec;
    std::filesystem::copy_file(inputPath, tempPath, std::filesystem::copy_options::overwrite_existing, ec);
    if (ec) {
        std::cerr << "Failed to prepare temp file: " << ec.message() << std::endl;
        return 1;
    }

    auto basePath = std::filesystem::path(R"(C:\Program Files (x86)\GOG Galaxy\Games\Generic\)");
    std::filesystem::path mutablePath = tempPath;
    DosboxStagingReplacer::ScriptEditService::resolveRelativePathsForDosboxAutoExec(mutablePath, basePath);

    std::ifstream resultFile(tempPath);
    std::ifstream expectedFile(expectedPath);

    if (!resultFile.is_open() || !expectedFile.is_open()) {
        std::cerr << "Failed to open result or expected file" << std::endl;
        std::filesystem::remove(tempPath);
        return 1;
    }

    std::string resultContent((std::istreambuf_iterator<char>(resultFile)), std::istreambuf_iterator<char>());
    std::string expectedContent((std::istreambuf_iterator<char>(expectedFile)), std::istreambuf_iterator<char>());

    bool success = (resultContent == expectedContent);
    if (!success) {
        std::cerr << "resolveRelativePathsForDosboxAutoExec() output mismatch" << std::endl;
        std::cerr << "Expected:\n" << expectedContent << std::endl;
        std::cerr << "Got:\n" << resultContent << std::endl;
    }

    resultFile.close();
    std::filesystem::remove(tempPath);

    return success ? 0 : 1;
}
