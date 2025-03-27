//
// Created by Orill on 3/14/2025.
//

#ifndef INSTALLATIONINFO_H
#define INSTALLATIONINFO_H

namespace DosboxStagingReplacer {

#include <string>

    /*
     * InstallationInfo struct. Contains the information about an installed application
     */
    struct InstallationInfo {
        std::string applicationName;
        std::string installationPath;
        std::string source;
    };

} // DosboxStagingReplacer

#endif //INSTALLATIONINFO_H
