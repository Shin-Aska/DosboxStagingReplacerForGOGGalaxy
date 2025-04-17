//
// Created by Richard Orilla on 3/23/2025.
//

#include <cstdint>
#ifndef SQL_CPP
#define SQL_CPP

#include "SqlService.h"

namespace DosboxStagingReplacer {

    SqlService::SqlService(const std::string &connectionString) {
        if (!connectionString.empty())  {
            this->SqlService::openConnection(this->connectionString);
        }
    }

    void SqlService::openConnection(const std::string &connectionString) {
        this->connectionString = connectionString;
        this->connectedFlag = true;
    }

    void SqlService::reconnect() {
        this->closeConnection();
        if (!this->connectionString.empty()) {
            this->openConnection(this->connectionString);
        }
        else {
            std::cerr << "No connection string provided during a reconnect attempt" << std::endl;
        }
    }

    void SqlService::closeConnection() {
        if (this->connectedFlag) {
            this->connectedFlag = false;
        }
        else {
            std::cerr << "Connection is already closed" << std::endl;
        }
    }

    bool SqlService::isConnectionOpen() const {
        return this->connectedFlag;
    }

    void SqlService::executeQuery(const std::string &query, const std::unordered_map<std::string, std::any> &params) {
        if (this->connectedFlag) {
            throw SqlServiceException("Method not implemented");
        }
        throw SqlServiceException("Connection is not open");
    }

    SqlService::~SqlService() {
        if (this->connectedFlag) {
            this->SqlService::closeConnection();
        }
    }

    SqlLiteService::SqlLiteService(const std::string &connectionString) {
        if (!connectionString.empty()) {
            this->SqlLiteService::openConnection(connectionString);
        }
    }

    SqlLiteService::~SqlLiteService() {
        if (this->connectedFlag) {
            this->SqlLiteService::closeConnection();
        }
    }

    void SqlLiteService::openConnection(const std::string &connectionString) {
        if (!connectionString.empty()) {
            this->connectionString = connectionString;
            this->connection = sqlite3_open_v2(this->connectionString.c_str(), &this->db, SQLITE_OPEN_READWRITE, nullptr);
            if (this->connection != SQLITE_OK) {
                std::cerr << "Error opening database: " << sqlite3_errmsg(this->db) << std::endl;
                sqlite3_close(this->db);
                throw SqlLiteServiceException(sqlite3_errmsg(this->db));
            }
            this->connectedFlag = true;
        }
    }

    void SqlLiteService::executeQuery(const std::string &query,
                                      const std::unordered_map<std::string, std::any> &params) {
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

            if (const std::any &value = it->second; value.type() == typeid(int64_t)) {
                sqlite3_bind_int64(stmt, i, std::any_cast<int64_t>(value));
            }
            else if (value.type() == typeid(int)) {
                sqlite3_bind_int(stmt, i, std::any_cast<int>(value));
            } else if (value.type() == typeid(double)) {
                sqlite3_bind_double(stmt, i, std::any_cast<double>(value));
            } else if (value.type() == typeid(std::string)) {
                sqlite3_bind_text(stmt, i, std::any_cast<std::string>(value).c_str(), -1, SQLITE_TRANSIENT);
            } else if (value.type() == typeid(const char *)) {
                sqlite3_bind_text(stmt, i, std::any_cast<const char *>(value), -1, SQLITE_TRANSIENT);
            } else if (value.type() == typeid(nullptr)) {
                sqlite3_bind_null(stmt, i);
            } else if (value.type() == typeid(bool)) {
                sqlite3_bind_int(stmt, i, std::any_cast<bool>(value));
            } else {
                std::cerr << "Unsupported type for key: " << key << std::endl;
                sqlite3_finalize(stmt);
                throw SqlLiteServiceException("Unsupported parameter type");
            }
        }

        // Execute the statement
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            std::cerr << "Error executing query: " << sqlite3_errmsg(this->db) << std::endl;
            sqlite3_finalize(stmt);
            throw SqlLiteServiceException(sqlite3_errmsg(this->db));
        }
    }

    void SqlLiteService::closeConnection() {
        if (this->connectedFlag) {
            sqlite3_close(this->db);
            this->connectedFlag = false;
        } else {
            std::cerr << "Connection is already closed" << std::endl;
        }
    }

} // DosboxStagingReplacer

#endif // SQL_CPP