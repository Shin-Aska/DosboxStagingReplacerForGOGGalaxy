//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include <any>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "SQLService.h"

namespace DosboxStagingReplacer {

    enum class DataResultDataType {
        Number, String, Boolean
    };

    /**
     * @brief Abstract base class for statement parsers.
     * Provides an interface for parsing SQL statements into data objects.
     */
    class SqlDataResult {
    public:
        virtual ~SqlDataResult() = default;

        /**
         * Fills the object with the data from the statement.
         *
         * @param stmt The statement to fill the object with.
         * @param parameters The parameters to fill the object with.
         * @param engine The engine to use to fill the object.
         * @return std::any
         */
        virtual std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine);

        /**
         * Returns all attributes of the class and their values.
         * @return Vector of (attribute name, value, data type).
         */
        virtual std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const {
            return {};
        }
    };

    class SqlDataResultException : public std::exception {
    public:
        explicit SqlDataResultException(const char *message) : msg(message) {}
        SqlDataResultException(SqlDataResultException const &) noexcept = default;
        SqlDataResultException &operator=(SqlDataResultException const &) noexcept = default;
        ~SqlDataResultException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

    /**
     * @brief SqliteLastRowId class. Contains the information about the last row id.
     */
    class SqliteLastRowId: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;

        /**
         * @brief Fills the SqliteLastRowId object with data from the statement.
         * @param stmt Statement to fill the object with.
         * @param parameters Parameters to use.
         * @param engine SQL engine.
         * @return std::any
         */
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;

        /**
         * Returns all attributes of the object.
         *
         * @return Vector of (attribute name, value, data type).
         */
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const override {
            return {{"id", std::to_string(id), DataResultDataType::Number}};
        }
    };

    /**
     * @brief SqliteSchema class. Contains the information about a SQLite schema.
     */
    class SqliteSchema: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        std::string type;
        std::string name;
        std::string tbl_name;
        int rootpage;

        /**
         * @brief Fills the SqliteSchema object with data from the statement.
         * @param stmt Statement to fill the object with.
         * @param parameters Parameters to use.
         * @param engine SQL engine.
         * @return std::any
         */
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;

        /**
         * @brief Returns all attributes of the object.
         * @return Vector of (attribute name, value, data type).
         */
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const override {
            return {
                {"type", type, DataResultDataType::String},
                {"name", name, DataResultDataType::String},
                {"tbl_name", tbl_name, DataResultDataType::String},
                {"rootpage", std::to_string(rootpage), DataResultDataType::Number}
            };
        }
    };

    /**
     * @brief ProductDetails class. Contains the information about a GOG product.
     */
    class ProductDetails: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int productId;
        std::string title;
        std::string slug;
        int gogId;
        std::string releaseKey;
        std::string installationPath;
        std::string installationDate;

        /**
         * @brief Fills the ProductDetails object with data from the statement.
         * @param stmt Statement to fill the object with.
         * @param parameters Parameters to use.
         * @param engine SQL engine.
         * @return std::any
         */
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;

        /**
         * @brief Returns all attributes of the object.
         * @return Vector of (attribute name, value, data type).
         */
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const override {
            return {
                {"productId", std::to_string(productId), DataResultDataType::Number},
                {"title", title, DataResultDataType::String},
                {"slug", slug, DataResultDataType::String},
                {"gogId", std::to_string(gogId), DataResultDataType::Number},
                {"releaseKey", releaseKey, DataResultDataType::String},
                {"installationPath", installationPath, DataResultDataType::String},
                {"installationDate", installationDate, DataResultDataType::String}
            };
        }
    };

    /**
     * @brief GogUser class. Contains the information about a GOG user.
     */
    class GogUser: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;

        /**
         * @brief Fills the GogUser object with data from the statement.
         * @param stmt Statement to fill the object with.
         * @param parameters Parameters to use.
         * @param engine SQL engine.
         * @return std::any
         */
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;

        /**
         * Returns all attributes of the object.
         *
         * @return Vector of (attribute name, value, data type).
         */
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const override {
            return {{"id", std::to_string(id), DataResultDataType::Number}};
        }
    };

    /**
     * @brief PlayTaskInformation class. Contains the information about a play task.
     */
    class PlayTaskInformation: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;
        std::string gameReleaseKey;
        int userId;
        int order;
        int typeId;
        std::string type;
        bool isPrimary;

        /**
         * @brief Fills the PlayTaskInformation object with data from the statement.
         * @param stmt Statement to fill the object with.
         * @param parameters Parameters to use.
         * @param engine SQL engine.
         * @return std::any
         */
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;

        /**
         * @brief Returns all attributes of the object.
         * @return Vector of (attribute name, value, data type).
         */
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const override {
            return {
                {"id", std::to_string(id), DataResultDataType::Number},
                {"gameReleaseKey", gameReleaseKey, DataResultDataType::String},
                {"userId", std::to_string(userId), DataResultDataType::Number},
                {"order", std::to_string(order), DataResultDataType::Number},
                {"typeId", std::to_string(typeId), DataResultDataType::Number},
                {"type", type, DataResultDataType::String},
                {"isPrimary", isPrimary ? "true" : "false", DataResultDataType::Boolean}
            };
        }
    };

    /**
     * @brief PlayTaskLaunchParameters class. Contains the information about a play task launch parameters.
     */
    class PlayTaskLaunchParameters: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int playTaskId;
        std::string executablePath;
        std::string commandLineArgs;
        std::string label;

        /**
         * @brief Fills the PlayTaskLaunchParameters object with data from the statement.
         * @param stmt Statement to fill the object with.
         * @param parameters Parameters to use.
         * @param engine SQL engine.
         * @return std::any
         */
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;

        /**
         * @brief Returns all attributes of the object.
         * @return Vector of (attribute name, value, data type).
         */
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> getAttributes() const override {
            return {
                {"playTaskId", std::to_string(playTaskId), DataResultDataType::Number},
                {"executablePath", executablePath, DataResultDataType::String},
                {"commandLineArgs", commandLineArgs, DataResultDataType::String},
                {"label", label, DataResultDataType::String}
            };
        }
    };

    /**
     * @brief Base class for parsing SQL statements.
     */
    class StatementParser {
    public:
        StatementParser();
        virtual ~StatementParser();

        /**
         * @brief Parses a SQL result into a SqlDataResult object.
         * @param result The SqlDataResult object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(SqlDataResult& result, std::vector<std::string> parameters, std::any stmt) = 0;

        /**
         * @brief Parses a SQL result into a SqliteLastRowId object.
         * @param result The SqliteLastRowId object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(SqliteLastRowId& result, std::vector<std::string> parameters, std::any stmt) = 0;
        /**
         * @brief Parses a SQL result into a SqliteSchema object.
         * @param result The SqliteSchema object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmt) = 0;
        /**
         * @brief Parses a SQL result into a ProductDetails object.
         * @param result The ProductDetails object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(ProductDetails& result, std::vector<std::string> parameters, std::any stmt) = 0;
        /**
         * @brief Parses a SQL result into a GogUser object.
         * @param result The GogUser object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(GogUser& result, std::vector<std::string> parameters, std::any stmt) = 0;
        /**
         * @brief Parses a SQL result into a PlayTaskInformation object.
         * @param result The PlayTaskInformation object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(PlayTaskInformation& result, std::vector<std::string> parameters, std::any stmt) = 0;
        /**
         * @brief Parses a SQL result into a PlayTaskLaunchParameters object.
         * @param result The PlayTaskLaunchParameters object to fill.
         * @param parameters The parameters to use.
         * @param stmt The statement to parse.
         */
        virtual void parseInto(PlayTaskLaunchParameters& result, std::vector<std::string> parameters, std::any stmt) = 0;
    };

    class SqliteStatementParser: public StatementParser {
    public:
        SqliteStatementParser();
        ~SqliteStatementParser() override;

        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::SqlDataResult&, std::vector<std::string>, std::any)
        void parseInto(SqlDataResult& result, std::vector<std::string> parameters, std::any stmtAny) override;
        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::SqliteLastRowId&, std::vector<std::string>, std::any)
        void parseInto(SqliteLastRowId& result, std::vector<std::string> parameters, std::any stmtAny) override;
        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::SqliteSchema&, std::vector<std::string>, std::any)
        void parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmtAny) override;
        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::ProductDetails&, std::vector<std::string>, std::any)
        void parseInto(ProductDetails& result, std::vector<std::string> parameters, std::any stmtAny) override;
        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::GogUser&, std::vector<std::string>, std::any)
        void parseInto(GogUser& result, std::vector<std::string> parameters, std::any stmtAny) override;
        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::PlayTaskInformation&, std::vector<std::string>, std::any)
        void parseInto(PlayTaskInformation& result, std::vector<std::string> parameters, std::any stmtAny) override;
        //! \copydoc DosboxStagingReplacer::StatementParser::parseInto(DosboxStagingReplacer::PlayTaskLaunchParameters&, std::vector<std::string>, std::any)
        void parseInto(PlayTaskLaunchParameters& result, std::vector<std::string> parameters, std::any stmtAny) override;

    };

    /**
     * @brief Factory class for creating StatementParser objects.
     */
    class StatementParserFactory {
    public:
        /**
         * Creates a StatementParser object based on the SqlEngine.
         *
         * @param engine SQL engine type.
         * @return A unique_ptr to a StatementParser.
         */
        static std::unique_ptr<StatementParser> createParser(SqlEngine engine);
    };

    class StatementEngineParserFactoryException: public std::exception {
    public:
        explicit StatementEngineParserFactoryException(const char *message) : msg(message) {}
        StatementEngineParserFactoryException(StatementEngineParserFactoryException const &) noexcept = default;
        StatementEngineParserFactoryException &operator=(StatementEngineParserFactoryException const &) noexcept = default;
        ~StatementEngineParserFactoryException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

} // namespace DosboxStagingReplacer

#endif // STATEMENTPARSER_H
