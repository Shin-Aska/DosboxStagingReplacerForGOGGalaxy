//
// Created by Richard Orilla on 3/24/2025.
//

#include <algorithm>

#include "DirectoryScanner.h"
#include "GogGalaxyService.h"

namespace DosboxStagingReplacer {

    void GogGalaxyService::setConnectionString(const std::string &connectionString) {
        this->openConnection(connectionString);
    }

    GogGalaxyService::GogGalaxyService(const std::string &connectionString) {
        if (!connectionString.empty()) {
            this->openConnection(connectionString);
        }
    }

    void GogGalaxyService::openConnection(const std::string &connectionString) {
        if (this->sqlService.isConnectionOpen()) {
            this->sqlService.closeConnection();
        }
        this->sqlService.openConnection(connectionString);
        if (!this->verifyDatabase()) {
            std::cerr << "Database verification failed. The database is not a valid GOG Galaxy database." << std::endl;
        }
    }

    void GogGalaxyService::closeConnection() {
        if (this->sqlService.isConnectionOpen()) {
            this->sqlService.closeConnection();
        } else {
            std::cerr << "Attempted to close a connection that is not open" << std::endl;
        }
    }


    void GogGalaxyService::disableAllPlayTaskFor(const std::string &gameReleaseKey) {
        if (this->validDatabase) {
            this->sqlService.executeQuery(R"SQL(
                UPDATE PlayTasks
                SET isPrimary = 0
                WHERE gameReleaseKey = :gameReleaseKey;
            ")SQL",
                                          {{"gameReleaseKey", gameReleaseKey}});
        } else {
            throw GogGalaxyServiceException("Database connection is not open");
        }
    }

    PlayTaskInformation GogGalaxyService::insertPlayTask(int64_t userId, int new_order,
                                                         const PlayTaskInformation &playTask) {
        if (this->validDatabase) {
            // Insert the new PlayTask into the database
            // Also get the id of the new PlayTask
            this->sqlService.executeQuery(R"SQL(
                INSERT INTO PlayTasks (gameReleaseKey, userId, "order", typeId, isPrimary)
                VALUES (:gameReleaseKey, :userId, :new_order, :typeId, :isPrimary);
            ")SQL",
                                          {{"gameReleaseKey", playTask.gameReleaseKey},
                                           {"userId", userId},
                                           {"new_order", new_order},
                                           {"typeId", playTask.typeId},
                                           {"isPrimary", playTask.isPrimary}});

            // Get the id of the new PlayTask
            const auto result = this->sqlService.executeQuery<SqliteLastRowId>(R"SQL(
                SELECT last_insert_rowid() AS id;
            )SQL",
                                                                               {});

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
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    void GogGalaxyService::insertPlayTaskLaunchParameters(const PlayTaskInformation &playTask,
                                                          const PlayTaskLaunchParameters &launchParameters) {
        if (this->validDatabase) {
            this->sqlService.executeQuery(R"SQL(
                INSERT INTO PlayTaskLaunchParameters (playTaskId, executablePath, commandLineArgs, label)
                VALUES (:playTaskId, :executablePath, :commandLineArgs, :label);
            )SQL",
                                          {{"playTaskId", playTask.id},
                                           {"executablePath", launchParameters.executablePath},
                                           {"commandLineArgs", launchParameters.commandLineArgs},
                                           {"label", launchParameters.label}});
        } else {
            throw GogGalaxyServiceException("Database connection is not open");
        }
    }


    bool GogGalaxyService::verifyDatabase() {
        if (this->sqlService.isConnectionOpen()) {
            const auto result = this->sqlService.executeQuery<SqliteSchema>(R"SQL(
                SELECT
                    type,
                    name,
                    tbl_name,
                    rootpage
                FROM sqlite_schema
                WHERE name IN
                ('Product Details View', 'ProductsToReleaseKeys', 'InstalledBaseProducts');
            )SQL",
                                                                            {});
            this->validDatabase = result.size() == 3;
            return this->validDatabase;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    bool GogGalaxyService::isDatabaseValid() const { return this->validDatabase; }

    std::vector<ProductDetails> GogGalaxyService::getProducts(const std::optional<std::string> &releaseKey,
                                                              const bool showDosOnly) {
        if (this->validDatabase) {
            std::string query = R"SQL(
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
                                ON ibp.productId = pdv.productId
            )SQL";

            std::unordered_map<std::string, std::any> params;

            if (releaseKey.has_value()) {
                query += " WHERE ptr.releaseKey = :releaseKey;";
                params.insert({"releaseKey", releaseKey.value()});
            }
            else {
                query += ";";
            }

            auto result = this->sqlService.executeQuery<ProductDetails>(query, params);

            // If showDosOnly is true, filter the results to only include DOS games
            // To do this, we use DirectoryScanner to get all the listed files in installationPath
            // and if it contains the folder DOSBOX, we add it to the result
            if (showDosOnly) {
                std::vector<ProductDetails> filteredResult;

                for (const auto &product: result) {
                    try {
                        if (auto filesInPath = DirectoryScanner::scanDirectory(product.installationPath);
                        std::ranges::any_of(filesInPath, [](const auto &file) {
                            return file.path.find("DOSBOX") != std::string::npos;
                        })) {
                            filteredResult.push_back(product);
                        }
                    } catch (const std::exception &e) {
                        std::cerr << "Error scanning directory: " << e.what() << std::endl;
                    }
                }

                return filteredResult;
            }
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
    std::vector<PlayTaskType> GogGalaxyService::getPlayTaskTypes() {
        if (this->validDatabase) {
            auto result = this->sqlService.executeQuery<PlayTaskType>(R"SQL(
                SELECT
                    id, type
                FROM PlayTaskTypes;
            )SQL",
                                                                      {});
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
            )SQL",
                                                                             {});
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
            )SQL",
                                                                             {{"gameReleaseKey", gameReleaseKey}});
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
            )SQL",
                                                                                  {});
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
            )SQL",
                                                                                  {{"playTaskId", playTaskId}});
            return result;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    void GogGalaxyService::addPlayTask(const int64_t userId, const std::string &gameReleaseKey,
                                       const PlayTaskInformation &playTask,
                                       const PlayTaskLaunchParameters &launchParameters) {
        if (this->validDatabase) {
            // Get all PlayTasks under the given gameReleaseKey
            auto existingPlayTasks = this->getPlayTasksFromGameReleaseKey(gameReleaseKey);
            int max_order = -1;

            // Get the maximum order of the existing PlayTasks
            for (const auto &task: existingPlayTasks) {
                if (task.order > max_order)
                    max_order = task.order;
            }

            // If there are no existing PlayTasks, set max_order to 1
            // This will only become -1 if there are no PlayTasks at all
            max_order = max_order == -1 ? 1 : max_order + 1;

            // Set all existing PlayTasks to not primary
            this->disableAllPlayTaskFor(gameReleaseKey);
            // Add the new PlayTask, get the updated PlayTask
            const auto updatedPlayTask = this->insertPlayTask(userId, max_order, playTask);
            // Add the PlayTaskLaunchParameters
            this->insertPlayTaskLaunchParameters(updatedPlayTask, launchParameters);
            return;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

    void GogGalaxyService::setCustomLaunchParametersForProduct(const std::string &gameReleaseKey, const bool enabled) {
        if (this->validDatabase) {
            this->sqlService.executeQuery(R"SQL(
                UPDATE ProductSettings
                SET customLaunchParameters = :enabled
                WHERE gameReleaseKey = :releaseKey;
            )SQL", {{"enabled", enabled}, {"releaseKey", gameReleaseKey}});
            return;
        }
        throw GogGalaxyServiceException("Database connection is not open");
    }

} // namespace DosboxStagingReplacer
