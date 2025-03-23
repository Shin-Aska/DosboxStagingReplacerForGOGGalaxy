//
// Created by Orill on 3/14/2025.
//

#ifndef DOSBOXVERIFIER_H
#define DOSBOXVERIFIER_H

#include <utility>

#include "InstallationVerifier.cpp"

namespace DosboxStagingReplacer {

    class DosboxVerifier: public InstallationVerifier {

        using InstallationVerifier::InstallationVerifier;

        public:
            bool isInstallationValid();
    };

} // DosboxStagingReplacer

#endif //DOSBOXVERIFIER_H
