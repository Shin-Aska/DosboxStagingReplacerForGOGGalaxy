//
// Created by Orill on 3/23/2025.
//

#ifndef BASE_H
#define BASE_H

#include <any>
#include <string>
#include <vector>
#include "../../services/sql/sql.cpp"
#include "../../factories/StatementEngineParserFactory.cpp"

namespace DosboxStagingReplacer {

    class BaseResult {
    public:
        virtual std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine);
    };

    class BaseResultException : public std::exception {
    public:
        explicit BaseResultException(const char *message) : msg(message) {}
        BaseResultException(BaseResultException const &) noexcept = default;
        BaseResultException &operator=(BaseResultException const &) noexcept = default;
        ~BaseResultException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

} // DosboxStagingReplacer

#endif //BASE_H
