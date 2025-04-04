#ifndef DB_AUTHENTICATION_H
#define DB_AUTHENTICATION_H

#include <string>

// ����� ���� �Լ�
// username�� hashedPassword(�ؽ� ó���� ��й�ȣ)�� �޾� DB�� Users ���̺��� 
// �ش� ������� �ؽ� ���� ���Ͽ� ��ġ�ϸ� true, �ƴϸ� false�� ��ȯ�մϴ�.
bool authenticateUser(const std::string& username, const std::string& hashedPassword);

// ����� ȸ������ �Լ� (�ɼ�)
// username�� password�� �޾�, ��й�ȣ�� �ؽ� ó���� �� DB�� �� ����� ������ INSERT �մϴ�.
// �����ϸ� true, �����ϸ� false�� ��ȯ�մϴ�.
bool registerUser(const std::string& username, const std::string& password);

#endif // DB_AUTHENTICATION_H
