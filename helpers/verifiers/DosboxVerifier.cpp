//
// Created by Orill on 3/14/2025.
//

#ifndef DOSBOXVERIFIER_CPP
#define DOSBOXVERIFIER_CPP

#include "DosboxVerifier.h"

namespace DosboxStagingReplacer {

    bool DosboxVerifier::isInstallationValid() {
        return InstallationVerifier::isInstallationValid();
    }
} // DosboxStagingReplacer

#endif //DOSBOXVERIFIER_CPP