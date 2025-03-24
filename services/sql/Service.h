//
// Created by Orill on 3/23/2025.
//

#pragma once

#ifndef SQL_H
#define SQL_H

#include <any>
#include <unordered_map>
#include <string>
#include <vector>
#include "sqlite3.h"

namespace DosboxStagingReplacer {

    class SqlService {
    protected:
        std::string connectionString = "";
        bool connectedFlag = false;

    public:
        explicit SqlService(std::string connectionString = "");
        virtual ~SqlService();
        virtual void openConnection(const std::string &connectionString);
        virtual void reconnect();
        virtual void closeConnection();
        bool isConnectionOpen() const;
        template<typename T> std::vector<T> executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params, bool withResult = false);
    };

    enum class SqlEngine {
        SQLITE
    };

    class SqlServiceException : public std::exception {
    public:
        explicit SqlServiceException(const char *message) : msg(message) {}
        SqlServiceException(SqlServiceException const &) noexcept = default;
        SqlServiceException &operator=(SqlServiceException const &) noexcept = default;
        ~SqlServiceException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

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

#endif // SQL_H
