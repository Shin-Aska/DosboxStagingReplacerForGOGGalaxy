//
// Created by Richard Orilla on 3/23/2025.
//

#include "StatementParser.h"

namespace DosboxStagingReplacer {

    std::unique_ptr<StatementParser> StatementParserFactory::createParser(const SqlEngine engine) {
        switch (engine) {
            case SqlEngine::SQLITE:
                return std::make_unique<SqliteStatementParser>();
            default:
                throw StatementEngineParserFactoryException("Unsupported engine");
        }
    }

    std::any SqlDataResult::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters, const SqlEngine engine) {
        throw SqlDataResultException("Method not implemented");
    }

    std::any SqliteLastRowId::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters, const SqlEngine engine) {
        const auto parser = StatementParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any SqliteSchema::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters, const SqlEngine engine) {
        const auto parser = StatementParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any ProductDetails::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters, const SqlEngine engine) {
        const auto parser = StatementParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any GogUser::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters, const SqlEngine engine) {
        const auto parser = StatementParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any PlayTaskInformation::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters,
                                                    const SqlEngine engine) {
        const auto parser = StatementParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any PlayTaskLaunchParameters::fillFromStatement(const std::any stmt, const std::vector<std::string> parameters,
                                                         const SqlEngine engine) {
        const auto parser = StatementParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    StatementParser::StatementParser() = default;
    StatementParser::~StatementParser() = default;

    SqliteStatementParser::SqliteStatementParser() = default;
    SqliteStatementParser::~SqliteStatementParser() = default;

    void SqliteStatementParser::parseInto(SqlDataResult &result, std::vector<std::string> parameters,
                                          const std::any stmtAny) {
        throw SqlDataResultException("Method not implemented");
    }

    void SqliteStatementParser::parseInto(SqliteLastRowId &result, std::vector<std::string> parameters, const std::any stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            if (std::string columnName = sqlite3_column_name(stmt, i);
                columnName == "id" || columnName == "last_insert_rowid()" || columnName == "\"last_insert_rowid()\"" ) {
                result.id = sqlite3_column_int(stmt, i);
            }
        }
    }

    void SqliteStatementParser::parseInto(SqliteSchema &result, std::vector<std::string> parameters, const std::any stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            if (std::string columnName = sqlite3_column_name(stmt, i); columnName == "type") {
                result.type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "name") {
                result.name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "tbl_name") {
                result.tbl_name = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "rootpage") {
                result.rootpage = sqlite3_column_int(stmt, i);
            }
        }
    }

    void SqliteStatementParser::parseInto(ProductDetails &result, std::vector<std::string> parameters,
                                          const std::any stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            if (std::string columnName = sqlite3_column_name(stmt, i); columnName == "productId") {
                result.productId = sqlite3_column_int(stmt, i);
            } else if (columnName == "title") {
                result.title = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "slug") {
                result.slug = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "gogId") {
                result.gogId = sqlite3_column_int(stmt, i);
            } else if (columnName == "releaseKey") {
                result.releaseKey = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "installationPath") {
                result.installationPath = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "installationDate") {
                result.installationDate = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            }
        }
    }

    void SqliteStatementParser::parseInto(GogUser &result, std::vector<std::string> parameters, const std::any stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            if (std::string columnName = sqlite3_column_name(stmt, i); columnName == "id") {
                result.id = sqlite3_column_int(stmt, i);
            }
        }
    }

    void SqliteStatementParser::parseInto(PlayTaskInformation &result, std::vector<std::string> parameters,
                                          const std::any stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            if (std::string columnName = sqlite3_column_name(stmt, i); columnName == "id") {
                result.id = sqlite3_column_int(stmt, i);
            } else if (columnName == "gameReleaseKey") {
                result.gameReleaseKey = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "userId") {
                result.userId = sqlite3_column_int(stmt, i);
            } else if (columnName == "order") {
                result.order = sqlite3_column_int(stmt, i);
            } else if (columnName == "typeId") {
                result.typeId = sqlite3_column_int(stmt, i);
            } else if (columnName == "type") {
                result.type = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "isPrimary") {
                result.isPrimary = sqlite3_column_int(stmt, i) != 0;
            }
        }
    }

    void SqliteStatementParser::parseInto(PlayTaskLaunchParameters &result, std::vector<std::string> parameters,
                                          const std::any stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            if (std::string columnName = sqlite3_column_name(stmt, i); columnName == "playTaskId") {
                result.playTaskId = sqlite3_column_int(stmt, i);
            } else if (columnName == "executablePath") {
                result.executablePath = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "commandLineArgs") {
                result.commandLineArgs = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            } else if (columnName == "label") {
                result.label = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
            }
        }
    }

} // namespace DosboxStagingReplacer
