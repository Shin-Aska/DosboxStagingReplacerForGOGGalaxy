//
// Created by Orill on 3/23/2025.
//

#ifndef DATA_CPP
#define DATA_CPP

#include "Data.h"
#include "../factories/StatementEngineParserFactory.cpp"

namespace DosboxStagingReplacer {

    std::any BaseResult::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        throw BaseResultException("Method not implemented");
    }

    std::any ProductDetails::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any SqliteSchema::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }
} // DosboxStagingReplacer

#endif // DATA_CPP
