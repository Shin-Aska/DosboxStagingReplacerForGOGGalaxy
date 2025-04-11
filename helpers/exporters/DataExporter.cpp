//
// Created by Orill on 4/12/2025.
//

#include "DataExporter.h"
#include <sstream>

namespace DosboxStagingReplacer {
    std::string DataExporter::serialize(std::vector<std::shared_ptr<SqlDataResult>> dataset) {
        std::ostringstream oss;
        for (const auto& data : dataset) {
            oss << this->stringify(*data) << std::endl;
        }
        return oss.str();
    }

    std::string DataExporter::serialize(std::vector<InstallationInfo> dataset) {
        std::ostringstream oss;
        for (const auto& data : dataset) {
            oss << this->stringify(data) << std::endl;
        }
        return oss.str();
    }

    std::string DataExporter::stringify(const SqlDataResult &data) {
        std::ostringstream oss;
        std::vector<std::tuple<std::string, std::string, std::string>> attributes = data.getAttributes();
        for (const auto &attribute : attributes) {
            std::string name, value, type;
            std::tie(name, value, type) = attribute;
            oss << name << "=" << value << this->separator;
        }
        return oss.str();
    }

    std::string DataExporter::stringify(const InstallationInfo &data) {
        std::ostringstream oss;
        oss << "applicationName=" << data.applicationName << this->separator
            << "installationPath=" << data.installationPath << this->separator
            << "source=" << data.source;
        return oss.str();
    }
} // DosboxStagingReplacer