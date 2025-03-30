//
// Created by Richard Orilla on 3/23/2025.
//

#include "Data.h"

namespace DosboxStagingReplacer {

    std::any SqlDataResult::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        throw SqlDataResultException("Method not implemented");
    }

    std::any SqliteLastRowId::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any SqliteSchema::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any ProductDetails::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any GogUser::fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any PlayTaskInformation::fillFromStatement(std::any stmt, std::vector<std::string> parameters,
                                                    SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }

    std::any PlayTaskLaunchParameters::fillFromStatement(std::any stmt, std::vector<std::string> parameters,
                                                    SqlEngine engine) {
        auto parser = StatementEngineParserFactory::createParser(engine);
        parser->parseInto(*this, parameters, stmt);
        return *this;
    }


} // DosboxStagingReplacer
