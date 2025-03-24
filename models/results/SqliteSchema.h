//
// Created by Orill on 3/23/2025.
//

#ifndef SQLITESCHEMA_H
#define SQLITESCHEMA_H

#include "base.cpp"

namespace DosboxStagingReplacer {

    class SqliteSchema: public BaseResult {
        using BaseResult::BaseResult;
    public:
        std::string type;
        std::string name;
        std::string tbl_name;
        int rootpage;
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

} // DosboxStagingReplacer

#endif //SQLITESCHEMA_H
