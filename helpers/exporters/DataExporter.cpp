#include "DataExporter.h"
#include <sstream>

namespace DosboxStagingReplacer {

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

    std::string JSONDataExporter::stringify(
            const std::vector<std::tuple<std::string, std::string, DataResultDataType>> &attributes) {
        std::ostringstream oss;
        oss << "{";
        for (size_t i = 0; i < attributes.size(); ++i) {
            const auto &[name, value, type] = attributes[i];

            // Currently we need to wrap strings in quotes
            std::string serializedValue = value;
            if (type == DataResultDataType::String) {
                serializedValue = addEscapeCharacters(serializedValue);
                serializedValue.insert(0, "\"");
                serializedValue.append("\"");
            }

            oss << "\"" << name << "\": " << serializedValue;
            if (i != attributes.size() - 1) {
                oss << ",";
            }
        }
        oss << "}";
        return oss.str();
    }

    std::string JSONDataExporter::formatLines(const std::vector<std::string> &lines) const {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < lines.size(); ++i) {
            oss << lines[i];
            if (i != lines.size() - 1) {
                oss << ",";
            }
        }
        oss << "]";
        return oss.str();
    }

    std::string CSVDataExporter::stringify(
            const std::vector<std::tuple<std::string, std::string, DataResultDataType>> &attributes) {
        std::ostringstream oss;
        for (size_t i = 0; i < attributes.size(); ++i) {
            const auto &[name, value, type] = attributes[i];
            oss << value;
            if (i != attributes.size() - 1) {
                oss << ",";
            }
        }
        return oss.str();
    }

} // namespace DosboxStagingReplacer
