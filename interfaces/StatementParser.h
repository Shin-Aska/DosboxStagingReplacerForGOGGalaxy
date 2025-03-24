//
// Created by Orill on 3/23/2025.
//

#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include <any>
#include "../models/Data.cpp"

namespace DosboxStagingReplacer {

    class StatementParser {
    public:
        StatementParser();
        ~StatementParser();
        virtual void parseInto(BaseResult& result, std::vector<std::string> parameters, std::any stmt) = 0;
        virtual void parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmt) = 0;
        virtual void parseInto(ProductDetails& result, std::vector<std::string> parameters, std::any stmt) = 0;
    };

    class SqliteStatementParser: public StatementParser {
        using StatementParser::StatementParser;
    public:
        SqliteStatementParser();
        ~SqliteStatementParser();
        void parseInto(BaseResult& result, std::vector<std::string> parameters, std::any stmtAny) override;
        void parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmtAny) override;
        void parseInto(ProductDetails& result, std::vector<std::string> parameters, std::any stmtAny) override;
    };

} // DosboxStagingReplacer


#endif //STATEMENTPARSER_H
