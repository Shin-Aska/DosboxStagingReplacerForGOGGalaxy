//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef STATEMENTENGINEPARSERFACTORY_CPP
#define STATEMENTENGINEPARSERFACTORY_CPP

#include "StatementEngineParserFactory.h"

namespace DosboxStagingReplacer {

    std::unique_ptr<StatementParser> StatementEngineParserFactory::createParser(SqlEngine engine) {
        switch (engine) {
            case SqlEngine::SQLITE:
                return std::make_unique<SqliteStatementParser>();
            default:
                throw StatementEngineParserFactoryException("Unsupported engine");
        }
    }
} // DosboxStagingReplacer

#endif // STATEMENTENGINEPARSERFACTORY_CPP