#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ReflectionUtils.h"

namespace DosboxStagingReplacer {

    /**
     * @brief Base class for exporting data from a dataset into a string format.
     */
    class DataExporter {
    public:
        /// @brief Constructor
        DataExporter() = default;

        /// @brief Destructor
        virtual ~DataExporter() = default;

        /**
         * @brief Serializes a homogeneous dataset into a string format.
         * @tparam R The range type of the dataset.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a string.
         */
        template <typename R>
        std::string serialize(R &dataset) {
            std::vector<std::string> lines;
            for (auto &data: dataset) {
                lines.push_back(stringify(reflectAttributes(data)));
            }
            return formatLines(lines);
        }

    protected:
        /**
         * @brief Converts a reflected attribute list into a string format.
         * @param attributes The reflected attributes (name, value, type) to convert.
         * @return The string representation of one record.
         */
        virtual std::string stringify(
                const std::vector<std::tuple<std::string, std::string, DataResultDataType>> &attributes) {
            std::ostringstream oss;
            for (const auto &[name, value, type]: attributes) {
                oss << name << "=" << value << ",";
            }
            return oss.str();
        }

        /**
         * @brief Combines serialized record lines into the final output.
         * @param lines The serialized records.
         * @return The combined output string.
         */
        [[nodiscard]] virtual std::string formatLines(const std::vector<std::string> &lines) const {
            std::ostringstream oss;
            for (size_t i = 0; i < lines.size(); ++i) {
                if (i != 0) {
                    oss << std::endl;
                }
                oss << lines[i];
            }
            return oss.str();
        }
    };

    /**
     * @brief Derived class for exporting data in JSON format.
     */
    class JSONDataExporter final : public DataExporter {
    protected:
        std::string stringify(
                const std::vector<std::tuple<std::string, std::string, DataResultDataType>> &attributes) override;

        [[nodiscard]] std::string formatLines(const std::vector<std::string> &lines) const override;

    private:
        static std::string addEscapeCharacters(const std::string &str);
    };

    /**
     * @brief Derived class for exporting data in CSV format.
     */
    class CSVDataExporter final : public DataExporter {
    protected:
        std::string stringify(
                const std::vector<std::tuple<std::string, std::string, DataResultDataType>> &attributes) override;
    };

    /**
     * @brief Factory class for creating DataExporter objects.
     */
    class DataExporterFactory {
    public:
        /**
         * @brief Creates a DataExporter object based on the specified type.
         * @param type The type of DataExporter to create (e.g., ".json", ".csv").
         * @return A unique pointer to the created DataExporter object.
         */
        static std::unique_ptr<DataExporter> createDataExporter(const std::string &type) {
            if (type == ".json") {
                return std::make_unique<JSONDataExporter>();
            }
            if (type == ".csv") {
                return std::make_unique<CSVDataExporter>();
            }
            return std::make_unique<DataExporter>();
        }
    };

} // namespace DosboxStagingReplacer

#endif // DATAEXPORTER_H
