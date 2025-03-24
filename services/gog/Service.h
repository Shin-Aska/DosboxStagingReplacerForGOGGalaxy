//
// Created by Orill on 3/24/2025.
//

#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "../sql/Service.cpp"
#include "../../models/Data.cpp"

namespace DosboxStagingReplacer {

    class GogGalaxyService {
    private:
        SqlLiteService sqlService;
        bool validDatabase = false;
    public:
        GogGalaxyService(std::string connectionString = "");
        ~GogGalaxyService() = default;

        bool verifyDatabase();
        bool isDatabaseValid() const;
        void setConnectionString(std::string connectionString);
        std::vector<ProductDetails> getProducts();
    };

    class GogGalaxyServiceException : public std::exception {
    public:
        explicit GogGalaxyServiceException(const char *message) : msg(message) {}
        GogGalaxyServiceException(GogGalaxyServiceException const &) noexcept = default;
        GogGalaxyServiceException &operator=(GogGalaxyServiceException const &) noexcept = default;
        ~GogGalaxyServiceException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

} // DosboxStagingReplacer

#endif //SERVICE_H
