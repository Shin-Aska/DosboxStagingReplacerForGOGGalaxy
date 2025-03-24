//
// Created by Orill on 3/23/2025.
//

#ifndef SQLITESCHEMA_CPP
#define SQLITESCHEMA_CPP

#include "SqliteSchema.h"

namespace DosboxStagingReplacer {
    std::any SqliteSchema::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }
} // namespace DosboxStagingReplacer

#endif // SQLITESCHEMA_CPP
