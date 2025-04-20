#include <iostream>
#include "InstallationVerifier.h"

int main () {
    // We first test InstallationVerifier::isInstallationValid() if it works
    // with a valid installation path and a valid target executable
    if (auto verifier = DosboxStagingReplacer::InstallationVerifier("../tests/data", "invalid.sqlite");
        verifier.isInstallationValid()) {
        std::cout << "InstallationVerifier::isInstallationValid() failed with a valid installation path and a valid target executable" << std::endl;
        return 1;
    }
    std::cout << "InstallationVerifier::isInstallationValid() passed with a valid installation path and a valid target executable" << std::endl;
    return 0;
}