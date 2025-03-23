//
// Created by Orill on 3/23/2025.
//

#ifndef SQL_H
#define SQL_H

namespace DosboxStagingReplacer {

#include <string>
#include <vector>
#include <optional>

    class SqlService {
    private:
        std::string connectionString = "";
        bool isConnectionOpen = false;
    public:
        explicit SqlService(std::optional<std::string> connectionString = std::nullopt);
        virtual ~SqlService();
        virtual void openConnection(const std::string &connectionString);
        virtual void reconnect();
        virtual void closeConnection();
        template <typename T> std::vector<T> executeQuery(const std::string &query, const bool withResult = false);
    };

} // DosboxStagingReplacer

#endif //SQL_H
