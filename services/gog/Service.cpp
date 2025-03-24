//
// Created by Orill on 3/24/2025.
//

#include "Service.h"

namespace DosboxStagingReplacer {

    void GogGalaxyService::setConnectionString(std::string connectionString) {
        if (this->sqlService.isConnectionOpen()) {
            this->sqlService.closeConnection();
        }
        this->sqlService.openConnection(connectionString);
        this->verifyDatabase();
    }

    GogGalaxyService::GogGalaxyService(std::string connectionString) {
        this->setConnectionString(connectionString);
    }

    bool GogGalaxyService::verifyDatabase() {
        if (this->sqlService.isConnectionOpen()) {
            auto result = this->sqlService.executeQuery<SqliteSchema>(R"SQL(
                SELECT
                    type,
                    name,
                    tbl_name,
                    rootpage
                FROM sqlite_schema
                WHERE name IN
                ('Product Details View', 'ProductsToReleaseKeys', 'InstalledBaseProducts');
            )SQL", {}, true);
            this->validDatabase = result.size() == 3;
            return this->validDatabase;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    bool GogGalaxyService::isDatabaseValid() const {
        return this->validDatabase;
    }

    std::vector<ProductDetails> GogGalaxyService::getProducts() {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<ProductDetails>(R"SQL(
                SELECT
                    pdv.productId,
                    pdv.title,
                    pdv.slug,
                    ptr.gogId,
                    ptr.releaseKey,
                    ibp.installationPath,
                    ibp.installationDate
                FROM [Product Details View] pdv
                     INNER JOIN ProductsToReleaseKeys ptr
                                ON ptr.gogId = pdv.productId
                     INNER JOIN InstalledBaseProducts ibp
                                ON ibp.productId = pdv.productId;
            )SQL", {}, true);
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

} // DosboxStagingReplacer