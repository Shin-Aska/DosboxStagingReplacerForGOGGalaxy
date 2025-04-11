//
// Created by Richard Orilla on 3/24/2025.
//

#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "SQLService.h"
#include "StatementParser.h"

namespace DosboxStagingReplacer {

    class GogGalaxyService {
    private:
        // Currently hardcoded to SqlLiteService instead of it's base class
        // I don't see the need for hot-swapping of Database engines yet
        SqlLiteService sqlService;
        bool validDatabase = false;

        void disableAllPlayTaskFor(const std::string& gameReleaseKey);
        PlayTaskInformation insertPlayTask(int userId, int new_order, const PlayTaskInformation &playTask);
        void insertPlayTaskLaunchParameters(const PlayTaskInformation &playTask, const PlayTaskLaunchParameters &launchParameters);
    public:
        explicit GogGalaxyService(const std::string &connectionString = "");
        ~GogGalaxyService() = default;

        void openConnection(const std::string &connectionString);
        void closeConnection();

        // Verifies if the Sqlite database is a valid GOG Galaxy database
        // @param void
        // @return bool
        bool verifyDatabase();
        // Verifies if the Sqlite database is a valid GOG Galaxy database.
        // @param void
        // @return bool
        [[nodiscard]] bool isDatabaseValid() const;
        // Sets the connection string for the Sqlite database
        // @param std::string connectionString: The connection string to set
        // @return void
        void setConnectionString(const std::string &connectionString);
        std::vector<ProductDetails> getProducts();
        std::vector<GogUser> getUsers();
        std::vector<PlayTaskInformation> getPlayTasks();
        std::vector<PlayTaskInformation> getPlayTasksFromGameReleaseKey(std::string gameReleaseKey);
        std::vector<PlayTaskLaunchParameters> getPlayTaskLaunchParameters();
        std::vector<PlayTaskLaunchParameters> getPlayTaskLaunchParametersFromPlayTaskId(int playTaskId);
        void addPlayTask(int userId, const std::string& gameReleaseKey, const PlayTaskInformation &playTask, const PlayTaskLaunchParameters &launchParameters);
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
