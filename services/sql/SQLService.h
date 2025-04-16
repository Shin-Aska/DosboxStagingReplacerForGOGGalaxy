//
// Created by Richard Orilla on 3/23/2025.
//

#pragma once

#ifndef SQL_H
#define SQL_H

#include <any>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>
#include "sqlite3.h"

namespace DosboxStagingReplacer {

    class SqlServiceException final : public std::exception {
    public:
        explicit SqlServiceException(const char *message) : msg(message) {}
        SqlServiceException(SqlServiceException const &) noexcept = default;
        SqlServiceException &operator=(SqlServiceException const &) noexcept = default;
        ~SqlServiceException() override = default;

        /// @brief Returns the exception message.
        [[nodiscard]] const char *what() const noexcept override { return msg; }

    private:
        const char *msg;
    };

    /**
     * @brief Abstract base class for database services.
     * Provides an interface for connecting to databases, executing queries, and handling connections.
     */
    class SqlService {
    protected:
        std::string connectionString;
        bool connectedFlag = false;

    public:
        /**
         * @brief Constructs a SqlService with an optional connection string.
         * @param connectionString The database connection string.
         */
        explicit SqlService(const std::string &connectionString = "");

        /// @brief Virtual destructor.
        virtual ~SqlService();

        /**
         * @brief Opens a connection to the database.
         * @param connectionString The connection string to use.
         */
        virtual void openConnection(const std::string &connectionString);

        /**
         * @brief Reconnects to the database.
         */
        virtual void reconnect();

        /**
         * @brief Closes the connection to the database.
         */
        virtual void closeConnection();

        /**
         * @brief Checks whether a database connection is open.
         * @return true if connection is open, false otherwise.
         */
        [[nodiscard]] bool isConnectionOpen() const;

        /**
         * @brief Executes a query and returns the results as a vector.
         * @tparam T The result data type.
         * @param query The SQL query string.
         * @param params Named parameters to bind in the query.
         * @return A vector of results of type T.
         */
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, const std::unordered_map<std::string, std::any>& params) {
            auto result = std::vector<T>();
            if (this->connectedFlag) {
                throw SqlServiceException("Method not implemented");
            }
            throw SqlServiceException("Connection is not open");
        }

        /**
         * @brief Executes a query that does not return results (e.g. INSERT, UPDATE).
         * @param query The SQL query string.
         * @param params Named parameters to bind in the query.
         */
        virtual void executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params);
    };

    /**
     * @brief Supported database engines.
     */
    enum class SqlEngine { SQLITE };

    class SqlLiteServiceException final : public std::exception {
    public:
        explicit SqlLiteServiceException(const char *message) : msg(message) {}
        SqlLiteServiceException(SqlLiteServiceException const &) noexcept = default;
        SqlLiteServiceException &operator=(SqlLiteServiceException const &) noexcept = default;
        ~SqlLiteServiceException() override = default;

        /// @brief Returns the exception message.
        [[nodiscard]] const char *what() const noexcept override { return msg; }

    private:
        const char *msg;
    };

    /**
     * @brief SQLite implementation of the SqlService interface.
     */
    class SqlLiteService final : public SqlService {
        using SqlService::SqlService;

        int connection = 0;
        sqlite3 *db = nullptr;

    public:
        /**
         * @brief Opens a SQLite database connection.
         * @param connectionString Path to the SQLite file.
         */
        void openConnection(const std::string &connectionString) override;

        /**
         * @brief Closes the SQLite database connection.
         */
        void closeConnection() override;

        /**
         * @brief Executes a query and maps results to objects.
         * @tparam T The result data type.
         * @param query The SQL query string.
         * @param params Named parameters to bind in the query.
         * @return A vector of deserialized objects of type T.
         */
        template<typename T>
        std::vector<T> executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params) {
            auto result = std::vector<T>();

            if (!this->connectedFlag) {
                throw SqlLiteServiceException("Connection is not open");
            }

            if (query.length() > static_cast<size_t>(std::numeric_limits<int>::max())) {
                throw std::length_error("SQL query string too long for sqlite3_prepare_v2");
            }

            sqlite3_stmt *stmt = nullptr;
            int rc = sqlite3_prepare_v2(this->db, query.c_str(), static_cast<int>(query.length()), &stmt, nullptr);
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

                if (const std::any &value = it->second; value.type() == typeid(int)) {
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
            columns.reserve(sqlite3_column_count(stmt));
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

        /**
         * @brief Executes a query that does not return results.
         * @param query The SQL query string.
         * @param params Named parameters to bind in the query.
         */
        void executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params) override;

        /**
         * @brief Constructs a SqlLiteService with an optional connection string.
         * @param connectionString The SQLite file path.
         */
        explicit SqlLiteService(const std::string &connectionString = "");

        /// @brief Destructor
        ~SqlLiteService() override;
    };

} // namespace DosboxStagingReplacer

#endif // SQL_H
