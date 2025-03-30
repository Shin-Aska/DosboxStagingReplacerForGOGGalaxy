//
// Created by Richard Orilla on 3/23/2025.
//

#pragma once

#ifndef SQL_H
#define SQL_H

#include <any>
#include <string>
#include <unordered_map>
#include <vector>
#include "sqlite3.h"

namespace DosboxStagingReplacer {

    /**
     *  SqlService class meant to be used as a base class for other database services. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for opening, closing, and
     *  executing queries on a database
     */
    class SqlService {
    protected:
        std::string connectionString = "";
        bool connectedFlag = false;

    public:
        explicit SqlService(const std::string &connectionString = "");
        virtual ~SqlService();
        // Opens a connection to the database. This is a pure virtual function
        // @param std::string connectionString: The connection string to use to connect to the database
        // @return void
        virtual void openConnection(const std::string &connectionString);
        // Reconnects to the database. This is a pure virtual function
        // @param void
        // @return void
        virtual void reconnect();
        // Closes the connection to the database. This is a pure virtual function
        // @param void
        // @return void
        virtual void closeConnection();
        // Checks if the connection is open. This relies on the connectedFlag variable which is set to true when the
        // openConnection function is called and set to false when the closeConnection function is called.
        // @param void
        // @return bool
        bool isConnectionOpen() const;
        // Executes a query on the database and returns a result. This is a pure virtual function
        // @param std::string query: The query to execute. This uses templates to automatically determine the return type
        // @param std::unordered_map<std::string, std::any> params: The parameters to use in the query
        // @param bool withResult: Whether the query should return a result
        // @return std::vector<T>
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params);
        virtual void executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params);
    };

    /*
     * SqlEngine enum class. Lists down the supported database engines
     */
    enum class SqlEngine { SQLITE };

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

    /**
     *  SqlLiteService class is the implementation of the SqlService class for the Sqlite database engine.
     */
    class SqlLiteService : public SqlService {

        using SqlService::SqlService;

        int connection = 0;
        sqlite3 *db = nullptr;

    public:
        // Sqlite implementation of opening a connection to the database
        // @param std::string connectionString: The connection string to use to connect to the database
        // @return void
        void openConnection(const std::string &connectionString) override;
        // Sqlite implementation of closing to the database
        // @param void
        // @return void
        void closeConnection() override;
        // Sqlite implementation of executing a query on the database and returning a result
        // @param std::string query: The query to execute. This uses templates to automatically determine the return type
        // @param std::unordered_map<std::string, std::any> params: The parameters to use in the query
        // @param bool withResult: Whether the query should return a result
        // @return std::vector<T>
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params);
        void executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params) override;
        explicit SqlLiteService(const std::string &connectionString = "");
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
