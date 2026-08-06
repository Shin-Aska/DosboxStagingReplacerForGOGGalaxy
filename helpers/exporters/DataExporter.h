#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <vector>

#include "CoreHelperModels.h"
#include "InstallationFinder.h"
#include "StatementParser.h"

namespace DosboxStagingReplacer {

    // Let us group all of these shit together!
    using DataSetVariant = std::variant<
        SqliteSchema,
        ProductDetails,
        GogUser,
        PlayTaskInformation,
        PlayTaskLaunchParameter,
        PlayTaskType,
        InstallationInfo,
        FileEntity
    >;

    /**
     * @brief Base class for exporting data from a dataset into a string format.
     * Although all the methods are virtual, they do have working implementations.
     */
    class DataExporter {
    public:
        /// @brief Constructor
        DataExporter() = default;

        /// @brief Destructor
        virtual ~DataExporter() = default;

        /**
         * @brief Serializes the DataSetVariant dataset into a string format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a string.
         */
        virtual std::string serialize(std::vector<DataSetVariant> &dataset);

        /**
         * @brief Converts the DataSetVariant object into a string format.
         * @param data The DataSetVariant (and its derivatives) object to convert.
         * @return The string representation of the DataSetVariant object.
         */
        virtual std::string stringify(DataSetVariant &data);
    private:
        std::string separator = ",";
    };

    /**
     * @brief Derived class for exporting data in JSON format.
     * Inherits from DataExporter and implements the serialization methods for JSON.
     */
    class JSONDataExporter final : public DataExporter {
    public:
        /**
         * @brief Serializes the DataSetVariant dataset into JSON format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a JSON string.
         */
        std::string serialize(std::vector<DataSetVariant> &dataset) override;

        /**
         * @brief Converts the DataSetVariant object into a JSON string.
         * @param data The DataSetVariant (and its derivatives) object to convert.
         * @return The string representation of the DataSetVariant object in JSON.
         */
        std::string stringify(DataSetVariant &data) override;

    private:
        /**
         * @brief Adds escape characters to a string to make it JSON-safe.
         * @param str The string to escape.
         * @return The escaped string.
         */
        static std::string addEscapeCharacters(const std::string &str);
    };

    /**
     * @brief Derived class for exporting data in CSV format.
     * Inherits from DataExporter and implements the serialization methods for CSV.
     */
    class CSVDataExporter final : public DataExporter {
    public:
        /**
         * @brief Serializes the DataSetVariant dataset into CSV format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a CSV string.
         */
        std::string serialize(std::vector<DataSetVariant> &dataset) override;
        /**
         * @brief Converts the DataSetVariant object into a CSV string.
         * @param data The DataSetVariant object to convert.
         * @return The string representation of the DataSetVariant object in CSV.
         */
        std::string stringify(DataSetVariant &data) override;
    private:
        std::string separator = ",";
    };

    /**
     * @brief Factory class for creating DataExporter objects.
     * This class is empty and serves as a placeholder for future implementations.
     */
    class DataExporterFactory {
    public:
        /**
         * @brief Creates a DataExporter object based on the specified type.
         * @param type The type of DataExporter to create (e.g., "json", "csv").
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
