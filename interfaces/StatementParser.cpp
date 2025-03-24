//
// Created by Orill on 3/23/2025.
//

#ifndef STATEMENTPARSER_CPP
#define STATEMENTPARSER_CPP

#include "StatementParser.h"

namespace DosboxStagingReplacer {
    StatementParser::StatementParser() { }
    StatementParser::~StatementParser() { }

    SqliteStatementParser::SqliteStatementParser() { }
    SqliteStatementParser::~SqliteStatementParser() { }

    void SqliteStatementParser::parseInto(BaseResult& result, std::vector<std::string> parameters, std::any stmtAny) {
        auto* stmt = std::any_cast<sqlite3_stmt*>(stmtAny);
    }

    void SqliteStatementParser::parseInto(SqliteSchema& result, std::vector<std::string> parameters, std::any stmtAny) {
        auto* stmt = std::any_cast<sqlite3_stmt*>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            std::string columnName = sqlite3_column_name(stmt, i);
            if (columnName == "type") {
                result.type = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "name") {
                result.name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "tbl_name") {
                result.tbl_name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "rootpage") {
                result.rootpage = sqlite3_column_int(stmt, i);
            }
        }
    }

    void SqliteStatementParser::parseInto(ProductDetails &result, std::vector<std::string> parameters, std::any stmtAny) {
        auto* stmt = std::any_cast<sqlite3_stmt*>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            std::string columnName = sqlite3_column_name(stmt, i);
            if (columnName == "productId") {
                result.productId = sqlite3_column_int(stmt, i);
            } else if (columnName == "title") {
                result.title = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "slug") {
                result.slug = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "gogId") {
                result.gogId = sqlite3_column_int(stmt, i);
            } else if (columnName == "releaseKey") {
                result.releaseKey = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            }
        }
    }
} // DosboxStagingReplacer


#endif // STATEMENTPARSER_CPP