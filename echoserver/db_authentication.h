#ifndef DB_AUTHENTICATION_H
#define DB_AUTHENTICATION_H

#include <string>

// 사용자 인증 함수
// username과 hashedPassword(해시 처리된 비밀번호)를 받아 DB의 Users 테이블에서 
// 해당 사용자의 해시 값과 비교하여 일치하면 true, 아니면 false를 반환합니다.
bool authenticateUser(const std::string& username, const std::string& hashedPassword);

// 사용자 회원가입 함수 (옵션)
// username과 password를 받아, 비밀번호는 해시 처리한 후 DB에 새 사용자 정보를 INSERT 합니다.
// 성공하면 true, 실패하면 false를 반환합니다.
bool registerUser(const std::string& username, const std::string& password);

#endif // DB_AUTHENTICATION_H
