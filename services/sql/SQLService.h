//
// Created by Richard Orilla on 3/23/2025.
//

#pragma once

#ifndef SQL_H
#define SQL_H

#include <any>
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>
#include "sqlite3.h"

namespace DosboxStagingReplacer {

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

        // PS: Currently the function definition is placed here as templates cannot be defined in a .cpp file because
        // of the way templates work in C++.
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, const std::unordered_map<std::string, std::any>& params) {
            auto result = std::vector<T>();
            if (this->connectedFlag) {
                throw SqlServiceException("Method not implemented");
            }
            throw SqlServiceException("Connection is not open");
        }
        virtual void executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params);
    };

    /*
     * SqlEngine enum class. Lists down the supported database engines
     */
    enum class SqlEngine { SQLITE };

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

        // PS: Currently the function definition is placed here as templates cannot be defined in a .cpp file because
        // of the way templates work in C++.
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params) {
            {
                auto result = std::vector<T>();

                if (!this->connectedFlag) {
                    throw SqlLiteServiceException("Connection is not open");
                }

                sqlite3_stmt *stmt = nullptr;
                int rc = sqlite3_prepare_v2(this->db, query.c_str(), query.length(), &stmt, nullptr);
                if (rc != SQLITE_OK) {
                    std::cerr << "Error preparing statement: " << sqlite3_errmsg(this->db) << std::endl;
                    sqlite3_finalize(stmt);
                    throw SqlLiteServiceException(sqlite3_errmsg(this->db));
                }

                // Bind parameters
                for (int i = 1; i <= sqlite3_bind_parameter_count(stmt); ++i) {
                    const char *paramName = sqlite3_bind_parameter_name(stmt, i);
                    if (!paramName)
                        continue;

                    std::string key = paramName;
                    if (key[0] == ':')
                        key = key.substr(1); // remove colon

                    auto it = params.find(key);
                    if (it == params.end()) {
                        std::cerr << "Missing value for SQL parameter: " << key << std::endl;
                        continue;
                    }

                    const std::any &value = it->second;

                    if (value.type() == typeid(int)) {
                        sqlite3_bind_int(stmt, i, std::any_cast<int>(value));
                    } else if (value.type() == typeid(double)) {
                        sqlite3_bind_double(stmt, i, std::any_cast<double>(value));
                    } else if (value.type() == typeid(std::string)) {
                        sqlite3_bind_text(stmt, i, std::any_cast<std::string>(value).c_str(), -1, SQLITE_TRANSIENT);
                    } else if (value.type() == typeid(const char *)) {
                        sqlite3_bind_text(stmt, i, std::any_cast<const char *>(value), -1, SQLITE_TRANSIENT);
                    } else if (value.type() == typeid(nullptr)) {
                        sqlite3_bind_null(stmt, i);
                    } else {
                        std::cerr << "Unsupported type for key: " << key << std::endl;
                        sqlite3_finalize(stmt);
                        throw SqlLiteServiceException("Unsupported parameter type");
                    }
                }

                std::vector<std::string> columns;
                for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
                    columns.emplace_back(sqlite3_column_name(stmt, i));
                }

                while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
                    auto datapoint_reference = T().fillFromStatement(stmt, columns, SqlEngine::SQLITE);
                    auto datapoint = std::any_cast<T&>(datapoint_reference);
                    result.push_back(datapoint);
                }

                if (rc != SQLITE_DONE) {
                    std::cerr << "Error executing query: " << sqlite3_errmsg(this->db) << std::endl;
                    sqlite3_finalize(stmt);
                    throw SqlLiteServiceException(sqlite3_errmsg(this->db));
                }

                sqlite3_finalize(stmt);
                return result;
            }
        }
        void executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params) override;
        explicit SqlLiteService(const std::string &connectionString = "");
        ~SqlLiteService() override;
    };


} // namespace DosboxStagingReplacer

#endif // SQL_H
