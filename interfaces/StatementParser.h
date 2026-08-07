//
// Created by Richard Orilla on 3/23/2025.
//

#ifndef STATEMENTPARSER_H
#define STATEMENTPARSER_H

#include <cstdint>
#include <meta>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace DosboxStagingReplacer {

    enum class DataResultDataType { Number, String, Boolean };

    class SqlDataResultException final : public std::exception {
    public:
        explicit SqlDataResultException(const char *message) : msg(message) {}
        SqlDataResultException(SqlDataResultException const &) noexcept = default;
        SqlDataResultException &operator=(SqlDataResultException const &) noexcept = default;
        ~SqlDataResultException() override = default;
        [[nodiscard]] const char *what() const noexcept override { return msg; }

    private:
        const char *msg;
    };

    namespace detail {
        /**
         * @brief Reflects the non-static data members of a type into a static array.
         * @tparam T - The type to reflect.
         * @return A static array containing the non-static data members of the type.
         */
        template <typename T>
        consteval auto reflected_member_span() {
            return std::define_static_array(
                    std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));
        }
    }

    /**
     * @brief Reflects attributes of an object into a vector of tuples.
     * @tparam T - The type of the object to reflect.
     * @param object - The object to reflect.
     * @return A vector of tuples containing attribute names, values, and data types.
     */
    template <typename T>
    std::vector<std::tuple<std::string, std::string, DataResultDataType>> reflectAttributes(const T &object) {
        std::vector<std::tuple<std::string, std::string, DataResultDataType>> result;
        static constexpr auto members = detail::reflected_member_span<T>();
        template for (constexpr auto member: members) {
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
            } else if constexpr (std::is_enum_v<FieldType>) {
                valueStr = std::to_string(static_cast<std::underlying_type_t<FieldType>>(value));
                typeTag = DataResultDataType::Number;
            } else {
                valueStr = std::to_string(value);
                typeTag = DataResultDataType::Number;
            }

            result.emplace_back(std::string(name), std::move(valueStr), typeTag);
        }
        return result;
    }

} // namespace DosboxStagingReplacer

#endif // STATEMENTPARSER_H
