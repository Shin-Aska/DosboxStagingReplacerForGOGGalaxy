//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include <any>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "SqlService.h"

namespace DosboxStagingReplacer {

    enum class DataResultDataType { Number, String, Boolean };

    class SqlDataResultException final : public std::exception {
    public:
        explicit SqlDataResultException(const char *message) : msg(message) {}
        SqlDataResultException(SqlDataResultException const &) noexcept = default;
        SqlDataResultException &operator=(SqlDataResultException const &) noexcept = default;
        ~SqlDataResultException() override = default;
        [[nodiscard]] const char *what() const noexcept override { return msg; }

    private:
        const char *msg;
    };

    /**
     * @brief SqliteLastRowId class. Contains the information about the last row id.
     */
    class SqliteLastRowId {
    public:
        int id;
    };

    /**
     * @brief SqliteSchema class. Contains the information about a SQLite schema.
     */
    class SqliteSchema final {
    public:
        std::string type;
        std::string name;
        std::string tbl_name;
        int rootpage;
    };

    /**
     * @brief ProductDetails class. Contains the information about a GOG product.
     */
    class ProductDetails {
    public:
        int productId;
        std::string title;
        std::string slug;
        int gogId;
        std::string releaseKey;
        std::string installationPath;
        std::string installationDate;
    };

    /**
     * @brief GogUser class. Contains the information about a GOG user.
     */
    class GogUser {
    public:
        int64_t id;
    };

    /**
     * @brief PlayTaskInformation class. Contains the information about a play task.
     */
    class PlayTaskInformation {
    public:
        int id;
        std::string gameReleaseKey;
        int userId;
        int order;
        int typeId;
        std::string type;
        bool isPrimary;
    };

    /**
     * @brief PlayTaskLaunchParameters class. Contains the information about a play task launch parameters.
     */
    class PlayTaskLaunchParameter {
    public:
        int playTaskId;
        std::string executablePath;
        std::string commandLineArgs;
        std::string label;

        bool operator==(const PlayTaskLaunchParameter &other) const {
            return executablePath == other.executablePath &&
                   commandLineArgs == other.commandLineArgs;
        }
    };

    /**
     * @brief PlayTaskType class. Contains the information about a play task type.
     */
    class PlayTaskType {
    public:
        int id;
        std::string type;
    };

    /**
     * @brief Reflects attributes of an object into a vector of tuples.
     * @tparam T - The type of the object to reflect.
     * @param object / The object to reflect.
     * @return A vector of tuples containing attribute names, values, and data types.
     */
    template <typename T>
    std::vector<std::tuple<std::string, std::string, DataResultDataType>> reflectAttributes(T &object);

    /**
     * @brief Parses an SQLite statement into a given object.
     * @tparam T - The type of the object to parse into.
     * @param result / The object to parse the statement into.
     * @param stmtAny / The SQLite statement to parse.
     */
    template <typename T>
    static void parseSqliteStatementInto(T &result, const std::any &stmtAny);


    /**
     * @brief Retrieves text data from a specific column in an SQLite statement.
     *
     * Extracts the UTF-8 encoded text value from the specified column of the provided SQLite statement.
     * If the column contains null data, an empty string is returned.
     *
     * @param stmt Pointer to the SQLite prepared statement object.
     * @param index Zero-based index of the column within the SQLite statement.
     * @return A string containing the text data from the specified column, or an empty string if the column is null.
     */
    std::string sqlite_column_text_or_empty(sqlite3_stmt*, int);

} // namespace DosboxStagingReplacer

#endif // STATEMENTPARSER_H
