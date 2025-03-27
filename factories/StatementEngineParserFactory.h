//
// Created by Orill on 3/23/2025.
//

#ifndef STATEMENTENGINEPARSERFACTORY_H
#define STATEMENTENGINEPARSERFACTORY_H

#include <any>
#include <memory>
#include "sqlite3.h"
#include "../services/sql/Service.cpp"
#include "../interfaces/StatementParser.cpp"

namespace DosboxStagingReplacer {

    /**
     *  StatementEngineParserFactory class is a factory class that creates a StatementParser object based on the SqlEngine
     */
    class StatementEngineParserFactory {
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

#endif //STATEMENTENGINEPARSERFACTORY_H
