//
// Created by Orill on 3/23/2025.
//

#ifndef SQL_H
#define SQL_H

#include <any>
#include <unordered_map>
#include <string>
#include <vector>

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

} // namespace DosboxStagingReplacer

#endif // SQL_H
