#include "InstallationFinder.h"

int main () {
    auto applications = DosboxStagingReplacer::getInstalledApplications();
    std::cout << "Found " << applications.size() << " installed applications" << std::endl;
    if (applications.empty())
        return 1;
    return 0;
}