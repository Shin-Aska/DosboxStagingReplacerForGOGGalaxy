//
// Created by Richard Orilla on 4/12/2025.
//

#include "DataExporter.h"

#include <format>
#include <sstream>

namespace DosboxStagingReplacer {
    std::string DataExporter::serialize(const std::vector<std::shared_ptr<SqlDataResult>> &dataset) {
        std::ostringstream oss;
        for (const auto &data: dataset) {
            oss << this->stringify(*data) << std::endl;
        }
        return oss.str();
    }

    std::string DataExporter::serialize(const std::vector<InstallationInfo> &dataset) {
        std::ostringstream oss;
        for (const auto &data: dataset) {
            oss << this->stringify(data) << std::endl;
        }
        return oss.str();
    }

    std::string DataExporter::stringify(const SqlDataResult &data) {
        std::ostringstream oss;
        for (std::vector<std::tuple<std::string, std::string, DataResultDataType>> attributes = data.getAttributes();
             const auto &attribute: attributes) {
            std::string name, value;
            DataResultDataType type;
            std::tie(name, value, type) = attribute;
            oss << name << "=" << value << this->separator;
        }
        return oss.str();
    }

    std::string DataExporter::stringify(const InstallationInfo &data) {
        std::ostringstream oss;
        oss << "applicationName=" << data.applicationName << this->separator
            << "installationPath=" << data.installationPath << this->separator << "source=" << data.source;
        return oss.str();
    }

    std::string JSONDataExporter::addEscapeCharacters(const std::string &str) {
        std::string escapedStr = str;
        // Escape backslashes
        size_t pos = 0;
        while ((pos = escapedStr.find('\\', pos)) != std::string::npos) {
            escapedStr.replace(pos, 1, "\\\\");
            pos += 2; // Move past the escaped backslash
        }
        // Escape double quotes
        pos = 0;
        while ((pos = escapedStr.find('\"', pos)) != std::string::npos) {
            escapedStr.replace(pos, 1, "\\\"");
            pos += 2; // Move past the escaped quote
        }
        return escapedStr;
    }

    std::string JSONDataExporter::serialize(const std::vector<std::shared_ptr<SqlDataResult>> &dataset) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < dataset.size(); ++i) {
            oss << this->stringify(*dataset[i]);
            if (i != dataset.size() - 1) {
                oss << ",";
            }
        }
        oss << "]";
        return oss.str();
    }

    std::string JSONDataExporter::serialize(const std::vector<InstallationInfo> &dataset) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < dataset.size(); ++i) {
            oss << this->stringify(dataset[i]);
            if (i != dataset.size() - 1) {
                oss << ",";
            }
        }
        oss << "]";
        return oss.str();
    }

    std::string JSONDataExporter::stringify(const SqlDataResult &data) {
        std::ostringstream oss;
        oss << "{";
        const std::vector<std::tuple<std::string, std::string, DataResultDataType>> attributes = data.getAttributes();
        for (size_t i = 0; i < attributes.size(); ++i) {
            std::string name, value;
            DataResultDataType type;
            std::tie(name, value, type) = attributes[i];

            // Currently we need to wrap strings in quotes
            if (type == DataResultDataType::String) {
                // First we replace all the quotes with escaped quotes
                value = addEscapeCharacters(value);
                // Then we add the quotes
                value.insert(0, "\"");
                value.append("\"");
            }

            oss << "\"" << name << "\": " << value;
            if (i != attributes.size() - 1) {
                oss << ",";
            }
        }
        oss << "}";
        return oss.str();
    }

    std::string JSONDataExporter::stringify(const InstallationInfo &data) {
        return std::format(R"({{"applicationName": "{}", "installationPath": "{}", "source": "{}"}})",
                           addEscapeCharacters(data.applicationName), addEscapeCharacters(data.installationPath),
                           addEscapeCharacters(data.source));
    }

    std::string CSVDataExporter::serialize(const std::vector<std::shared_ptr<SqlDataResult>> &dataset) {
        std::ostringstream oss;
        for (const auto &data: dataset) {
            oss << this->stringify(*data) << std::endl;
        }
        return oss.str();
    }

    std::string CSVDataExporter::serialize(const std::vector<InstallationInfo> &dataset) {
        std::ostringstream oss;
        for (const auto &data: dataset) {
            oss << this->stringify(data) << std::endl;
        }
        return oss.str();
    }

    std::string CSVDataExporter::stringify(const SqlDataResult &data) {
        std::ostringstream oss;
        for (std::vector<std::tuple<std::string, std::string, DataResultDataType>> attributes = data.getAttributes();
             const auto &attribute: attributes) {
            std::string name, value;
            DataResultDataType type;
            std::tie(name, value, type) = attribute;
            oss << value << this->separator;
        }
        return oss.str();
    }

    std::string CSVDataExporter::stringify(const InstallationInfo &data) {
        std::ostringstream oss;
        oss << data.applicationName << this->separator << data.installationPath << this->separator << data.source;
        return oss.str();
    }
} // namespace DosboxStagingReplacer
