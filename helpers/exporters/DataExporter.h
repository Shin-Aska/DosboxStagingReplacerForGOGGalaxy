//
// Created by Orill on 4/12/2025.
//

#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <vector>
#include "InstallationFinder.h"
#include "StatementParser.h"

namespace DosboxStagingReplacer {

    class DataExporter {
    private:
        std::string separator = ",";
    public:
        // Constructor
        DataExporter() = default;
        // Destructor
        virtual ~DataExporter() = default;

        // Serializes the SqlDataResult dataset into a string format
        // @params dataset: The dataset to serialize
        // @return std::string: The serialized dataset
        virtual std::string serialize(std::vector<std::shared_ptr<SqlDataResult>> dataset);
        virtual std::string serialize(std::vector<InstallationInfo> dataset);

        // Converts the SqlDataResult object into a string format
        // @params data: The SqlDataResult object to convert
        // @return std::string: The string representation of the SqlDataResult object
        virtual std::string stringify(const SqlDataResult &data);
        virtual std::string stringify(const InstallationInfo &data);
    };

} // namespace DosboxStagingReplacer

#endif // DATAEXPORTER_H
