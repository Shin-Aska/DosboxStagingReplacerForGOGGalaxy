//
// Created by Orill on 3/23/2025.
//

#ifndef SQL_CPP
#define SQL_CPP

#include "sql.h"
#include <iostream>

namespace DosboxStagingReplacer {

    SqlService::SqlService(std::string connectionString) {
        if (!connectionString.empty())  {
            this->openConnection(this->connectionString);
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

    template <typename T> std::vector<T> SqlService::executeQuery(const std::string &query, std::unordered_map<std::string, std::any> params, const bool withResult) {
        auto result = std::vector<T>();
        if (this->connectedFlag) {
            throw SqlServiceException("Method not implemented");
        }
        throw SqlServiceException("Connection is not open");
    }

    SqlService::~SqlService() {
        if (this->connectedFlag) {
            this->closeConnection();
        }
    }
} // DosboxStagingReplacer

#endif // SQL_CPP