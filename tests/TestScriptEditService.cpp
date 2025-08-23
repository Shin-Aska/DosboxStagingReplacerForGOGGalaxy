#include <iostream>
#include "ScriptEditService.h"

int main() {
    // Test case 1: Savage Empire paths
    const struct {
        std::string cmd = R"(-conf "..\dosboxSAVAGE.conf" -conf "..\dosboxSAVAGE_single.conf" -noconsole -c "exit")";
        std::filesystem::path path = R"(C:\Program Files (x86)\GOG Galaxy\Games\The Savage Empire\)";
        std::string expected =
                R"(-conf "C:\Program Files (x86)\GOG Galaxy\Games\The Savage Empire\dosboxSAVAGE.conf" -conf "C:\Program Files (x86)\GOG Galaxy\Games\The Savage Empire\dosboxSAVAGE_single.conf" -noconsole -c "exit")";
    } test1;

    // Test case 2: War of the Lance paths
    const struct {
        std::string cmd = "-noconsole -conf ..\\flame-base.conf -conf flame-graphics.conf -conf flame-game.conf";
        std::filesystem::path path = R"(C:\Program Files (x86)\GOG Galaxy\Games\War of the Lance)";
        std::string expected =
                R"(-noconsole -conf "C:\Program Files (x86)\GOG Galaxy\Games\War of the Lance\flame-base.conf" -conf "C:\Program Files (x86)\GOG Galaxy\Games\War of the Lance\flame-graphics.conf" -conf "C:\Program Files (x86)\GOG Galaxy\Games\War of the Lance\flame-game.conf")";
    } test2;

    const std::string result1 =
            DosboxStagingReplacer::ScriptEditService::resolveRelativePathsFromString(test1.cmd, test1.path);
    const std::string result2 =
            DosboxStagingReplacer::ScriptEditService::resolveRelativePathsFromString(test2.cmd, test2.path);

    bool failed = false;

    if (result1 != test1.expected) {
        std::cerr << "Test case 1 failed:" << std::endl;
        std::cerr << "Expected: " << test1.expected << std::endl;
        std::cerr << "Got: " << result1 << std::endl;
        failed = true;
    }

    if (result2 != test2.expected) {
        std::cerr << "Test case 2 failed:" << std::endl;
        std::cerr << "Expected: " << test2.expected << std::endl;
        std::cerr << "Got: " << result2 << std::endl;
        failed = true;
    }

    return failed ? 1 : 0;
}
