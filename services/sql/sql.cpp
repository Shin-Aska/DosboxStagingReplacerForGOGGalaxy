//
// Created by Orill on 3/23/2025.
//

#ifndef SQL_CPP
#define SQL_CPP

#include "sql.h"

namespace DosboxStagingReplacer {

    #include <iostream>

    SqlService::SqlService(std::optional<std::string> connectionString): connectionString(connectionString.value_or("")) {
        if (!connectionString.has_value())  {
            this->openConnection(this->connectionString);
        }
    }

    void SqlService::openConnection(const std::string &connectionString) {
        this->connectionString = connectionString;
        this->isConnectionOpen = true;
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
        if (this->isConnectionOpen) {
            this->isConnectionOpen = false;
        }
        else {
            std::cerr << "Connection is already closed" << std::endl;
        }
    }

    template <typename T> std::vector<T> SqlService::executeQuery(const std::string &query, const bool withResult) {
        auto result = std::vector<T>();
        if (this->isConnectionOpen) {
            return result;
        }
        else {
            std::cerr << "Connection is not open" << std::endl;
        }
    }

    SqlService::~SqlService() {
        if (this->isConnectionOpen) {
            this->closeConnection();
        }
    }
} // DosboxStagingReplacer

#endif // SQL_CPP