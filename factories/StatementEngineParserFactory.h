//
// Created by Orill on 3/23/2025.
//

#ifndef STATEMENTENGINEPARSERFACTORY_H
#define STATEMENTENGINEPARSERFACTORY_H

#include <any>
#include <memory>
#include "sqlite3.h"
#include "../../services/sql/sql.h"
#include "../interfaces/SqLiteStatementParser.cpp"

namespace DosboxStagingReplacer {

    class StatementEngineParserFactory {
    public:
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
