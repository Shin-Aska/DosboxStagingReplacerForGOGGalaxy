#include <StatementParser.h>
#include <iostream>
#include "SqlService.h"

int main () {

    std::cout << "Testing SqlLiteService::openConnection() with valid SQlite database" << std::endl;
    DosboxStagingReplacer::SqlLiteService validSqlService("../tests/data/valid.sqlite");

    std::cout << "Testing SqliteService::executeQuery()" << std::endl;

    const auto result = validSqlService.executeQuery<DosboxStagingReplacer::SqliteSchema>(R"SQL(
        SELECT
            type,
            name,
            tbl_name,
            rootpage
        FROM sqlite_schema
    )SQL", {});

    if (result.empty()) {
        std::cout << "SqliteService::executeQuery() failed" << std::endl;
        return 1;
    }

    std::cout << "SqliteService::executeQuery() passed" << std::endl;
    std::cout << "Testing SqliteService::executeQuery() with invalid Sqlite database" << std::endl;

    try {
        DosboxStagingReplacer::SqlLiteService invalidSqlService("../tests/data/invalid.sqlite");
        const auto result2 = validSqlService.executeQuery<DosboxStagingReplacer::SqliteSchema>(R"SQL(
            SELECT
                type,
                name,
                tbl_name,
                rootpage
            FROM non_existent_table
        )SQL", {});

        std::cout << "SqliteService::executeQuery() should fail with invalid database" << std::endl;
        return 1;
    } catch (const DosboxStagingReplacer::SqlLiteServiceException &e) {
        std::cout << "SqliteService::executeQuery() failed as expected: " << e.what() << std::endl;
    }

    return 0;
}