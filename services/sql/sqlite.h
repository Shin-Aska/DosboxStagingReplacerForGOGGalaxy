//
// Created by Orill on 3/23/2025.
//

#ifndef SQLITE_H
#define SQLITE_H

#include "sqlite3.h"

#include "sql.cpp"

namespace DosboxStagingReplacer {

    class SqlLiteService : public SqlService {

        using SqlService::SqlService;

        int connection = 0;
        sqlite3 *db = nullptr;

    public:
        void openConnection(const std::string &connectionString) override;
        void closeConnection() override;
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params,
                                    bool withResult = false);
        ~SqlLiteService() override;
    };

    class SqlLiteServiceException : public std::exception {
    public:
        explicit SqlLiteServiceException(const char *message) : msg(message) {}
        SqlLiteServiceException(SqlLiteServiceException const &) noexcept = default;
        SqlLiteServiceException &operator=(SqlLiteServiceException const &) noexcept = default;
        ~SqlLiteServiceException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

} // namespace DosboxStagingReplacer

#endif // SQLITE_H
