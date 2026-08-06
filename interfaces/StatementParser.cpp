#include "StatementParser.h"
#include <meta>

namespace DosboxStagingReplacer {

    [[nodiscard]] auto sqlite_column_text_or_empty(sqlite3_stmt *stmt, const int index) -> std::string {
        if (const auto *text = sqlite3_column_text(stmt, index)) {
            return std::string{reinterpret_cast<const char *>(text)};
        }
        return {};
    }

    template<typename T>
    std::vector<std::tuple<std::string, std::string, DataResultDataType>> reflectAttributes(T &object) {
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> result;
        template for (constexpr auto member: std::meta::nonstatic_data_members_of(^^T,
                                                                                  std::meta::access_context::unchecked())) {
            constexpr auto name = std::meta::identifier_of(member);
            const auto &value = object.[:member:];
            using FieldType = std::remove_cvref_t<decltype(value)>;

            std::string valueStr;
            DataResultDataType typeTag;

            if constexpr (std::is_same_v<FieldType, std::string>) {
                valueStr = value;
                typeTag = DataResultDataType::String;
            } else if constexpr (std::is_same_v<FieldType, bool>) {
                valueStr = value ? "true" : "false";
                typeTag = DataResultDataType::Boolean;
            } else {
                valueStr = std::to_string(value);
                typeTag = DataResultDataType::Number;
            }

            result.emplace_back(std::string(name), std::move(valueStr), typeTag);
        }
        return result;
    }

    template<typename T>
    void parseSqliteStatementInto(T &result, const std::any &stmtAny) {
        auto *stmt = std::any_cast<sqlite3_stmt *>(stmtAny);
        const int columnCount = sqlite3_column_count(stmt);

        for (int i = 0; i < columnCount; i++) {
            const std::string columnName = sqlite3_column_name(stmt, i);
            template for (const auto& member: std::meta::nonstatic_data_members_of(^^T,
                                                                                   std::meta::access_context::unchecked())) {
                if (constexpr auto memberName = std::meta::identifier_of(member);
                    columnName == std::string(memberName)) {
                    using FieldType = std::remove_cvref_t<typename [:std::meta::type_of(member):]>;
                    if constexpr (std::is_same_v<FieldType, std::string>) {
                        result.[:member:] = sqlite_column_text_or_empty(stmt, i);
                    } else if constexpr (std::is_same_v<FieldType, int64_t>) {
                        result.[:member:] = sqlite3_column_int64(stmt, i);
                    } else if constexpr (std::is_same_v<FieldType, int>) {
                        result.[:member:] = sqlite3_column_int(stmt, i);
                    } else if constexpr (std::is_same_v<FieldType, bool>) {
                        result.[:member:] = sqlite3_column_int(stmt, i) != 0;
                    }
                    break;
                }
            }
        }
    }

} // namespace DosboxStagingReplacer
