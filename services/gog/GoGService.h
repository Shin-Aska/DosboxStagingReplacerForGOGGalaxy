//
// Created by Richard Orilla on 3/24/2025.
//

#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include "SQLService.h"
#include "StatementParser.h"

namespace DosboxStagingReplacer {

    /**
     * @brief Provides operations for interacting with the GOG Galaxy database.
     *
     * This service handles CRUD operations on products, users, and play tasks
     * using the SQLite backend of the GOG Galaxy client.
     */
    class GogGalaxyService {
    private:
        SqlLiteService sqlService;
        bool validDatabase = false;

        void disableAllPlayTaskFor(const std::string& gameReleaseKey);
        PlayTaskInformation insertPlayTask(int userId, int new_order, const PlayTaskInformation &playTask);
        void insertPlayTaskLaunchParameters(const PlayTaskInformation &playTask, const PlayTaskLaunchParameters &launchParameters);

    public:
        /**
         * @brief Constructs the service with an optional database connection string.
         * @param connectionString Path to the SQLite database file.
         */
        explicit GogGalaxyService(const std::string &connectionString = "");

        /// @brief Destructor
        ~GogGalaxyService() = default;

        /**
         * @brief Opens a connection to the database.
         * @param connectionString The path to the database file.
         */
        void openConnection(const std::string &connectionString);

        /**
         * @brief Closes the active database connection.
         */
        void closeConnection();

        /**
         * @brief Verifies if the current database is a valid GOG Galaxy database.
         * @return true if valid, false otherwise.
         */
        bool verifyDatabase();

        /**
         * @brief Checks whether the current database is valid.
         * @return true if the database is valid, false otherwise.
         */
        [[nodiscard]] bool isDatabaseValid() const;

        /**
         * @brief Sets the database connection string.
         * @param connectionString The new connection string.
         */
        void setConnectionString(const std::string &connectionString);

        /**
         * @brief Retrieves all products in the database.
         * @return A vector of ProductDetails objects.
         */
        std::vector<ProductDetails> getProducts();

        /**
         * @brief Retrieves all users in the database.
         * @return A vector of GogUser objects.
         */
        std::vector<GogUser> getUsers();

        /**
         * @brief Retrieves all play tasks from the database.
         * @return A vector of PlayTaskInformation objects.
         */
        std::vector<PlayTaskInformation> getPlayTasks();

        /**
         * @brief Retrieves play tasks associated with a specific game release key.
         * @param gameReleaseKey The release key of the game.
         * @return A vector of PlayTaskInformation objects.
         */
        std::vector<PlayTaskInformation> getPlayTasksFromGameReleaseKey(std::string gameReleaseKey);

        /**
         * @brief Retrieves all play task launch parameters from the database.
         * @return A vector of PlayTaskLaunchParameters objects.
         */
        std::vector<PlayTaskLaunchParameters> getPlayTaskLaunchParameters();

        /**
         * @brief Retrieves launch parameters associated with a specific play task ID.
         * @param playTaskId The ID of the play task.
         * @return A vector of PlayTaskLaunchParameters objects.
         */
        std::vector<PlayTaskLaunchParameters> getPlayTaskLaunchParametersFromPlayTaskId(int playTaskId);

        /**
         * @brief Adds a new play task and its associated launch parameters.
         * @param userId ID of the user to assign the task to.
         * @param gameReleaseKey Release key of the game.
         * @param playTask The play task information.
         * @param launchParameters The associated launch parameters.
         */
        void addPlayTask(int userId, const std::string& gameReleaseKey, const PlayTaskInformation &playTask, const PlayTaskLaunchParameters &launchParameters);
    };

    /**
     * @brief Exception class for GOG Galaxy service errors.
     */
    class GogGalaxyServiceException : public std::exception {
    public:
        explicit GogGalaxyServiceException(const char *message) : msg(message) {}
        GogGalaxyServiceException(GogGalaxyServiceException const &) noexcept = default;
        GogGalaxyServiceException &operator=(GogGalaxyServiceException const &) noexcept = default;
        ~GogGalaxyServiceException() override = default;

        /// @brief Returns the exception message.
        const char *what() const noexcept override { return msg; }

    private:
        const char *msg;
    };

} // namespace DosboxStagingReplacer

#endif // SERVICE_H
