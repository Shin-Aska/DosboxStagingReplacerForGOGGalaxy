//
// Created by Richard Orilla on 3/24/2025.
//

#include "GoGService.h"

namespace DosboxStagingReplacer {

    void GogGalaxyService::setConnectionString(const std::string &connectionString) {
        if (this->sqlService.isConnectionOpen()) {
            this->sqlService.closeConnection();
        }
        this->sqlService.openConnection(connectionString);
        this->verifyDatabase();
    }

    GogGalaxyService::GogGalaxyService(const std::string &connectionString) {
        this->setConnectionString(connectionString);
    }

    void GogGalaxyService::disableAllPlayTaskFor(const std::string& gameReleaseKey) {
        if (this->validDatabase) {
            this->sqlService.executeQuery(R"SQL(
                UPDATE PlayTasks
                SET isPrimary = 0
                WHERE gameReleaseKey = :gameReleaseKey;
            ")SQL", {{"gameReleaseKey", gameReleaseKey}});
        } else {
            throw GogGalaxyServiceException("Database connection is not open");
        }
    }

    PlayTaskInformation GogGalaxyService::insertPlayTask(int userId, int new_order, const PlayTaskInformation &playTask) {
        if (this->validDatabase) {
            // Insert the new PlayTask into the database
            // Also get the id of the new PlayTask
            this->sqlService.executeQuery(R"SQL(
                INSERT INTO PlayTasks (gameReleaseKey, userId, "order", typeId, isPrimary)
                VALUES (:gameReleaseKey, :userId, :new_order, :typeId, :isPrimary);
            ")SQL", {
                {"gameReleaseKey", playTask.gameReleaseKey},
                {"userId", userId},
                {"new_order", new_order},
                {"typeId", playTask.typeId},
                {"isPrimary", playTask.isPrimary}
            });

            // Get the id of the new PlayTask
            auto result = this->sqlService.executeQuery<SqliteLastRowId>(R"SQL(
                SELECT last_insert_rowid() AS id;
            )SQL", {});

            // Check if the result is empty
            if (result.empty()) {
                throw GogGalaxyServiceException("Failed to get the id of the new PlayTask");
            }
            // Create a copy of the PlayTask object and set the id
            PlayTaskInformation newPlayTask = playTask;
            newPlayTask.id = std::any_cast<int>(result[0].id);
            newPlayTask.userId = userId;
            newPlayTask.order = new_order;
            return newPlayTask;
        } else {
            throw GogGalaxyServiceException("Database connection is not open");
        }
    }

    void GogGalaxyService::insertPlayTaskLaunchParameters(const PlayTaskInformation &playTask, const PlayTaskLaunchParameters &launchParameters) {
        if (this->validDatabase) {
            this->sqlService.executeQuery(R"SQL(
                INSERT INTO PlayTaskLaunchParameters (playTaskId, executablePath, commandLineArgs, label)
                VALUES (:playTaskId, :executablePath, :commandLineArgs, :label);
            )SQL", {
                {"playTaskId", playTask.id},
                {"executablePath", launchParameters.executablePath},
                {"commandLineArgs", launchParameters.commandLineArgs},
                {"label", launchParameters.label}
            });
        } else {
            throw GogGalaxyServiceException("Database connection is not open");
        }
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
            )SQL", {});
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
            )SQL", {});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    std::vector<GogUser> GogGalaxyService::getUsers() {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<GogUser>(R"SQL(
                SELECT
                    id
                FROM Users;
            )SQL", {});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    std::vector<PlayTaskInformation> GogGalaxyService::getPlayTasks() {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<PlayTaskInformation>(R"SQL(
                SELECT
                    pt.id, pt.gameReleaseKey, pt.userId, pt."order", pt.typeId, ptt.type, pt.isPrimary
                FROM PlayTasks pt
                INNER JOIN PlayTaskTypes ptt ON pt.typeId = ptt.id;
            )SQL", {});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    std::vector<PlayTaskInformation> GogGalaxyService::getPlayTasksFromGameReleaseKey(std::string gameReleaseKey) {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<PlayTaskInformation>(R"SQL(
                SELECT
                    pt.id, pt.gameReleaseKey, pt.userId, pt."order", pt.typeId, ptt.type, pt.isPrimary
                FROM PlayTasks pt
                INNER JOIN PlayTaskTypes ptt ON pt.typeId = ptt.id
                WHERE pt.gameReleaseKey = :gameReleaseKey;
            )SQL", {{"gameReleaseKey", gameReleaseKey}});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    std::vector<PlayTaskLaunchParameters> GogGalaxyService::getPlayTaskLaunchParameters() {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<PlayTaskLaunchParameters>(R"SQL(
                SELECT
                    ptlp.playTaskId, ptlp.executablePath, ptlp.commandLineArgs, ptlp.label
                FROM PlayTaskLaunchParameters ptlp
            )SQL", {});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    std::vector<PlayTaskLaunchParameters> GogGalaxyService::getPlayTaskLaunchParametersFromPlayTaskId(int playTaskId) {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<PlayTaskLaunchParameters>(R"SQL(
                SELECT
                    ptlp.playTaskId, ptlp.executablePath, ptlp.commandLineArgs, ptlp.label
                FROM PlayTaskLaunchParameters ptlp
                WHERE ptlp.playTaskId = :playTaskId;
            )SQL", {{"playTaskId", playTaskId}});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    void GogGalaxyService::addPlayTask(int userId, const std::string& gameReleaseKey, const PlayTaskInformation &playTask, const PlayTaskLaunchParameters &launchParameters) {
        if (this->validDatabase) {
            // Get all PlayTasks under the given gameReleaseKey
            auto existingPlayTasks = this->getPlayTasksFromGameReleaseKey(gameReleaseKey);
            int max_order = -1;

            // Get the maximum order of the existing PlayTasks
            for (const auto &task : existingPlayTasks) {
                if (task.order > max_order) {
                    max_order = task.order;
                }
            }

            // If there are no existing PlayTasks, set max_order to 1
            // This will only become -1 if there are no PlayTasks at all
            max_order = (max_order == -1) ? 1 : max_order + 1;

            // Set all existing PlayTasks to not primary
            this->disableAllPlayTaskFor(gameReleaseKey);
            // Add the new PlayTask, get the updated PlayTask
            const PlayTaskInformation updatedPlayTask = this->insertPlayTask(userId, max_order, playTask);
            // Add the PlayTaskLaunchParameters
            this->insertPlayTaskLaunchParameters(updatedPlayTask, launchParameters);
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

} // DosboxStagingReplacer