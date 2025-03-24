//
// Created by Orill on 3/23/2025.
//

#ifndef SQLITESTATEMENTPARSER_H
#define SQLITESTATEMENTPARSER_H

#include "StatementParser.cpp"

namespace DosboxStagingReplacer {

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

#endif //SQLITESTATEMENTPARSER_H
