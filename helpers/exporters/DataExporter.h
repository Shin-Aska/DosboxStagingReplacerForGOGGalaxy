//
// Created by Richard Orilla on 3/14/2025.
//

#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <vector>
#include "InstallationFinder.h"
#include "StatementParser.h"

namespace DosboxStagingReplacer {

    /**
     * @brief Base class for exporting data from a dataset into a string format.
     * Although all the methods are virtual, they do have working implementations.
     */
    class DataExporter {
    private:
        std::string separator = ",";
    public:
        /// @brief Constructor
        DataExporter() = default;

        /// @brief Destructor
        virtual ~DataExporter() = default;

        /**
         * @brief Serializes the SqlDataResult dataset into a string format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a string.
         */
        virtual std::string serialize(const std::vector<std::shared_ptr<SqlDataResult>> &dataset);

        /**
         * @brief Serializes the InstallInfo dataset into a string format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a string.
         */
        virtual std::string serialize(const std::vector<InstallationInfo> &dataset);

        /**
         * @brief Converts the SqlDataResult object into a string format.
         * @param data The object to convert.
         * @return The string representation of the SqlDataResult object.
         */
        virtual std::string stringify(const SqlDataResult &data);

        /**
         * @brief Converts the InstallationInfo object into a string format.
         * @param data The object to convert.
         * @return The string representation of the InstallationInfo object.
         */
        virtual std::string stringify(const InstallationInfo &data);
    };

    /**
     * @brief Derived class for exporting data in JSON format.
     * Inherits from DataExporter and implements the serialization methods for JSON.
     */
    class JSONDataExporter final : public DataExporter {

        /**
         * @brief Adds escape characters to a string to make it JSON-safe.
         * @param str The string to escape.
         * @return The escaped string.
         */
        static std::string addEscapeCharacters(const std::string &str);

        /**
         * @brief Serializes the SqlDataResult dataset into JSON format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a JSON string.
         */
        std::string serialize(const std::vector<std::shared_ptr<SqlDataResult>> &dataset) override;

        /**
         * @brief Serializes the InstallationInfo dataset into JSON format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a JSON string.
         */
        std::string serialize(const std::vector<InstallationInfo> &dataset) override;

        /**
         * @brief Converts the SqlDataResult object into a JSON string.
         * @param data The object to convert.
         * @return The string representation of the SqlDataResult object in JSON.
         */
        std::string stringify(const SqlDataResult &data) override;

        /**
         * @brief Converts the InstallationInfo object into a JSON string.
         * @param data The object to convert.
         * @return The string representation of the InstallationInfo object in JSON.
         */
        std::string stringify(const InstallationInfo &data) override;
    };

    class CSVDataExporter final : public DataExporter {
    private:
        std::string separator = ",";
    public:
        /**
         * @brief Serializes the SqlDataResult dataset into JSON format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a JSON string.
         */
        std::string serialize(const std::vector<std::shared_ptr<SqlDataResult>> &dataset) override;

        /**
         * @brief Serializes the InstallationInfo dataset into JSON format.
         * @param dataset The dataset to serialize.
         * @return The serialized dataset as a JSON string.
         */
        std::string serialize(const std::vector<InstallationInfo> &dataset) override;

        /**
         * @brief Converts the SqlDataResult object into a JSON string.
         * @param data The object to convert.
         * @return The string representation of the SqlDataResult object in JSON.
         */
        std::string stringify(const SqlDataResult &data) override;

        /**
         * @brief Converts the InstallationInfo object into a JSON string.
         * @param data The object to convert.
         * @return The string representation of the InstallationInfo object in JSON.
         */
        std::string stringify(const InstallationInfo &data) override;
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
            } else if (type == ".csv") {
                return std::make_unique<CSVDataExporter>();
            } else {
                return std::make_unique<DataExporter>();
            }
        }
    };

} // namespace DosboxStagingReplacer

#endif // DATAEXPORTER_H
