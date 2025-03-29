//
// Created by Orill on 3/23/2025.
//

#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include <any>
#include "../models/Data.cpp"

namespace DosboxStagingReplacer {

    /**
     *  StatementParser class meant to be used as a base class for other statement parsers. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for parsing the result of a
     *  statement into an object
     */
    class StatementParser {
    public:
        StatementParser();
        ~StatementParser();
        // Sql Statement Parser with the intention of parsing the result into the SqlDataResult object. This is a pure virtual function
        // @params SqlDataResult& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        virtual void parseInto(SqlDataResult& result, std::vector<std::string> parameters, std::any stmt) = 0;
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
        ~SqliteStatementParser();
        // Sqlite Statement Parser with the intention of parsing the result into the SqlDataResult object.
        // @params SqlDataResult& result - The result object that will be populated with the parsed data
        // @params std::vector<std::string> parameters - The parameters that will be used to parse the data
        // @params std::any stmt - The statement that will be used to parse the data
        // @return void
        void parseInto(SqlDataResult& result, std::vector<std::string> parameters, std::any stmtAny) override;
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

} // DosboxStagingReplacer


#endif //STATEMENTPARSER_H
