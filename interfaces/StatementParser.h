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

    /**
     *  SqlDataResult class meant to be used as a base class for other result classes. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for filling an object with data
     *  from a statement but also represents
     */
    class SqlDataResult {
    public:
        virtual ~SqlDataResult() = default;
        // Fills the object with the data from the statement, depending on the parameters and the engine provided
        // With the intention of the object obtaining the data from the statement. This is a pure virtual function
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        virtual std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine);
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values. This is a virtual function meant to be overridden by the derived classes
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        virtual std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const {
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

    class SqliteLastRowId: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;
        // Implementation of filling the SqliteLastRowId object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values.
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const override {
            return {{"id", std::to_string(id), "Number"}};
        }
    };

    /**
     * The data class representation of the SqliteSchema in a Sqlite database.
     */
    class SqliteSchema: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        std::string type;
        std::string name;
        std::string tbl_name;
        int rootpage;
        // Implementation of filling the SqliteSchema object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values.
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const override {
            return {
                {"type", type, "String"},
                {"name", name, "String"},
                {"tbl_name", tbl_name, "String"},
                {"rootpage", std::to_string(rootpage), "Number"}
            };
        }
    };

    /**
     * The data class representations of the Products in the Gog Galaxy database.
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
        // Implementation of filling the ProductDetails object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values.
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const override {
            return {
                {"productId", std::to_string(productId), "Number"},
                {"title", title, "String"},
                {"slug", slug, "String"},
                {"gogId", std::to_string(gogId), "Number"},
                {"releaseKey", releaseKey, "String"},
                {"installationPath", installationPath, "String"},
                {"installationDate", installationDate, "String"}
            };
        }
    };

    /**
     * The data class representation of a User in the Gog Galaxy database.
     */
    class GogUser: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;

        // Implementation of filling the GogUser object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values.
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const override {
            return {
                {"id", std::to_string(id), "Number"}
            };
        }
    };

    /**
     * The data class representation of the PlayTask in the Gog Galaxy database.
     * We have added a join statement to PlayTaskType and get the type name as well to lessen the number of queries
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

        // Implementation of filling the PlayTaskInformation object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values.
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const override {
            return {
                {"id", std::to_string(id), "Number"},
                {"gameReleaseKey", gameReleaseKey, "String"},
                {"userId", std::to_string(userId), "Number"},
                {"order", std::to_string(order), "Number"},
                {"typeId", std::to_string(typeId), "Number"},
                {"type", type, "String"},
                {"isPrimary", isPrimary ? "true" : "false", "Boolean"}
            };
        }
    };

    /**
     * The data class representation of the PlayTaskLaunchParameters in the Gog Galaxy database.
     */
    class PlayTaskLaunchParameters: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int playTaskId;
        std::string executablePath;
        std::string commandLineArgs;
        std::string label;

        // Implementation of filling the PlayTaskLaunchParameters object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
        // Manual implementation of reflection, where the class will return all the attributes of the class and their
        // values.
        // @return std::vector<std::tuple<std::string, std::string, std::string>>: A vector of tuples
        // First element is the name of the attribute, second is the value and the third is the type of the attribute
        std::vector<std::tuple<std::string, std::string, std::string>> getAttributes() const override {
            return {
                {"playTaskId", std::to_string(playTaskId), "Number"},
                {"executablePath", executablePath, "String"},
                {"commandLineArgs", commandLineArgs, "String"},
                {"label", label, "String"}
            };
        }
    };

    /**
     *  StatementParser class meant to be used as a base class for other statement parsers. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for parsing the result of a
     *  statement into an object
     */
    class StatementParser {
    public:
        StatementParser();
        virtual ~StatementParser();
        // Sql Statement Parser with the intention of parsing the result into the SqlDataResult object. This is a pure virtual function
        // @params SqlDataResult& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(SqlDataResult& result, std::vector<std::string> parameters, std::any stmt) = 0;
        // Sql Statement Parser with the intention of parsing the result into the SqliteLastRowId object. This is a pure virtual function
        // @params SqliteLastRowId& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(SqliteLastRowId& result, std::vector<std::string> parameters, std::any stmt) = 0;
        // Sql Statement Parser with the intention of parsing the result into the SqliteSchema object. This is a pure virtual function
        // @params SqliteSchema& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmt) = 0;
        // Sql Statement Parser with the intention of parsing the result into the ProductDetails object. This is a pure virtual function
        // @params ProductDetails& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(ProductDetails& result, std::vector<std::string> parameters, std::any stmt) = 0;
        // Sql Statement Parser with the intention of parsing the result into the GogUser object. This is a pure virtual function
        // @params GogUser& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(GogUser& result, std::vector<std::string> parameters, std::any stmt) = 0;
        // Sql Statement Parser with the intention of parsing the result into the PlayTaskInformation object. This is a pure virtual function
        // @params PlayTaskInformation& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(PlayTaskInformation& result, std::vector<std::string> parameters, std::any stmt) = 0;
        // Sql Statement Parser with the intention of parsing the result into the PlayTaskLaunchParameters object. This is a pure virtual function
        // @params PlayTaskLaunchParameters& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(PlayTaskLaunchParameters& result, std::vector<std::string> parameters, std::any stmt) = 0;
    };

    /**
     *  SqliteStatementParser class is the implementation of the StatementParser class for the Sqlite database engine.
     */
    class SqliteStatementParser: public StatementParser {
        using StatementParser::StatementParser;
    public:
        SqliteStatementParser();
        ~SqliteStatementParser() override;
        // Sqlite Statement Parser with the intention of parsing the result into the SqlDataResult object.
        // @params SqlDataResult& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(SqlDataResult& result, std::vector<std::string> parameters, std::any stmtAny) override;
        // Sqlite Statement Parser with the intention of parsing the result into the SqliteLastRowId object.
        // @params SqliteLastRowId& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(SqliteLastRowId& result, std::vector<std::string> parameters, std::any stmtAny) override;
        // Sqlite Statement Parser with the intention of parsing the result into the SqliteSchema object.
        // @params SqliteSchema& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmtAny) override;
        // Sqlite Statement Parser with the intention of parsing the result into the ProductDetails object.
        // @params ProductDetails& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(ProductDetails& result, std::vector<std::string> parameters, std::any stmtAny) override;
        // Sqlite Statement Parser with the intention of parsing the result into the GogUser object.
        // @params GogUser& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(GogUser& result, std::vector<std::string> parameters, std::any stmtAny) override;
        // Sqlite Statement Parser with the intention of parsing the result into the PlayTaskInformation object.
        // @params PlayTaskInformation& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(PlayTaskInformation& result, std::vector<std::string> parameters, std::any stmtAny) override;
        // Sqlite Statement Parser with the intention of parsing the result into the PlayTaskLaunchParameters object.
        // @params PlayTaskLaunchParameters& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(PlayTaskLaunchParameters& result, std::vector<std::string> parameters, std::any stmtAny) override;
    };

    /**
     *  StatementParserFactory class is a factory class that creates a StatementParser object based on the SqlEngine
     */
    class StatementParserFactory {
    public:
        // Create a StatementParser object based on the SqlEngine
        // @param SqlEngine engine: The SqlEngine to create the StatementParser object for
        // @return std::unique_ptr<StatementParser>
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

} // DosboxStagingReplacer


#endif //STATEMENTPARSER_H
