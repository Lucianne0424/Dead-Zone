#include "db_authentication.h"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <iostream>
#include <stdexcept>

bool authenticateUser(const std::string& username, const std::string& hashedPassword) {
    try {
        sql::Driver* driver = get_driver_instance();
        std::unique_ptr<sql::Connection> con(
            driver->connect("tcp://127.0.0.1:3306", "root", "cys07301")
        );
        con->setSchema("mygamedb");
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("SELECT hashed_password FROM users WHERE username = ?")
        );
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            std::string storedHash = res->getString("hashed_password");
            return (storedHash == hashedPassword);
        }
        else {
            return false;
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "[authenticateUser] SQL Exception: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "[authenticateUser] Unknown exception occurred" << std::endl;
        return false;
    }
}

bool registerUser(const std::string& username, const std::string& password) {
    try {
      
        sql::Driver* driver = get_driver_instance();
        std::unique_ptr<sql::Connection> con(
            driver->connect("tcp://127.0.0.1:3306", "root", "cys07301")
        );
        con->setSchema("mygamedb");
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement("INSERT INTO users(username, hashed_password) VALUES(?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, password); 
        int affectedRows = pstmt->executeUpdate();
        return (affectedRows > 0);
    }
    catch (sql::SQLException& e) {
        std::cerr << "[registerUser] SQL Exception: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "[registerUser] Unknown exception occurred" << std::endl;
        return false;
    }
}
