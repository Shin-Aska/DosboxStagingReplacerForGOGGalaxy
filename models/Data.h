//
// Created by Richard Orilla on 3/24/2025.
//

#ifndef DATA_H
#define DATA_H

#include <any>
#include <string>
#include <vector>
#include "../services/sql/Service.cpp"

namespace DosboxStagingReplacer {

    /**
     *  SqlDataResult class meant to be used as a base class for other result classes. This class is meant to be
     *  inherited from and not used directly. This class provides the basic framework for filling an object with data
     *  from a statement but also represents
     */
    class SqlDataResult {
    public:
        virtual ~SqlDataResult() = default;
        // Fills the object with the data from the statement, depending on the parameters and the engine provided
        // With the intention of the object obtaining the data from the statement. This is a pure virtual function
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        virtual std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine);
    };

    class SqlDataResultException : public std::exception {
    public:
        explicit SqlDataResultException(const char *message) : msg(message) {}
        SqlDataResultException(SqlDataResultException const &) noexcept = default;
        SqlDataResultException &operator=(SqlDataResultException const &) noexcept = default;
        ~SqlDataResultException() override = default;
        const char *what() const noexcept override { return msg; }
    private:
        const char *msg;
    };

    class SqliteLastRowId: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;
        // Implementation of filling the SqliteLastRowId object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

    /**
     * The data class representation of the SqliteSchema in a Sqlite database.
     */
    class SqliteSchema: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        std::string type;
        std::string name;
        std::string tbl_name;
        int rootpage;
        // Implementation of filling the SqliteSchema object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

    /**
     * The data class representations of the Products in the Gog Galaxy database.
     */
    class ProductDetails: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int productId;
        std::string title;
        std::string slug;
        int gogId;
        std::string releaseKey;
        std::string installationPath;
        std::string installationDate;
        // Implementation of filling the ProductDetails object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

    /**
     * The data class representation of a User in the Gog Galaxy database.
     */
    class GogUser: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;

        // Implementation of filling the GogUser object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

    /**
     * The data class representation of the PlayTask in the Gog Galaxy database.
     * We have added a join statement to PlayTaskType and get the type name as well to lessen the number of queries
     */
    class PlayTaskInformation: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int id;
        std::string gameReleaseKey;
        int userId;
        int order;
        int typeId;
        std::string type;
        bool isPrimary;

        // Implementation of filling the PlayTaskInformation object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

    /**
     * The data class representation of the PlayTaskLaunchParameters in the Gog Galaxy database.
     */
    class PlayTaskLaunchParameters: public SqlDataResult {
        using SqlDataResult::SqlDataResult;
    public:
        int playTaskId;
        std::string executablePath;
        std::string commandLineArgs;
        std::string label;

        // Implementation of filling the PlayTaskLaunchParameters object with the data from the statement, depending on the
        // parameters and the engine provided.
        // @param std::any stmt: The statement to fill the object with
        // @param std::vector<std::string> parameters: The parameters to fill the object with
        // @param SqlEngine engine: The engine to use to fill the object
        // @return std::any
        std::any fillFromStatement(std::any stmt, std::vector<std::string> parameters, SqlEngine engine) override;
    };

} // DosboxStagingReplacer

#endif //DATA_H
