//
// Created by Orill on 3/24/2025.
//

#ifndef DATA_H
#define DATA_H

#include <any>
#include <string>
#include <vector>
#include "../services/sql/Service.cpp"

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

    class ProductDetails: public BaseResult {
        using BaseResult::BaseResult;
    public:
        std::string productId;
        std::string title;
        std::string slug;
        int gogId;
        std::string releaseKey;
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

    class SqliteSchema: public BaseResult {
        using BaseResult::BaseResult;
    public:
        std::string type;
        std::string name;
        std::string tbl_name;
        int rootpage;
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

} // DosboxStagingReplacer

#endif //DATA_H
