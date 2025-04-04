// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "Game.h"
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <limits>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
WindowInfo GWindowInfo;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// 글로벌 네트워크 소켓
SOCKET g_clientSocket = INVALID_SOCKET;

// 로그인 상태 플래그 (테스트용: 로그인 명령 자동 전송 후)
std::atomic<bool> g_loggedIn(false);

// 게임 시작 플래그 (서버가 "GAME_START" 신호를 보내면 설정)
std::atomic<bool> g_gameStarted(false);

// 함수 선언:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// 네트워크 초기화 함수
bool InitNetwork() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::wcerr << L"WSAStartup 실패: " << result << std::endl;
        return false;
    }
    g_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_clientSocket == INVALID_SOCKET) {
        std::wcerr << L"소켓 생성 실패: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(g_clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::wcerr << L"서버 연결 실패: " << WSAGetLastError() << std::endl;
        closesocket(g_clientSocket);
        WSACleanup();
        return false;
    }
    std::wcout << L"네트워크 초기화 및 서버 연결 성공" << std::endl;
    return true;
}

// ReceiverThread: 서버 응답을 지속적으로 수신
void ReceiverThread(SOCKET clientSocket) {
    char buffer[8192];
    while (true) {
        int recvResult = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recvResult > 0) {
            buffer[recvResult] = '\0';
            std::string msg(buffer);
            std::cout << "\n[서버 응답] " << msg << std::endl;
            // 로그인 응답 처리
            if (msg.find("LOGIN_OK") != std::string::npos || msg.find("SIGNUP_OK") != std::string::npos) {
                g_loggedIn = true;
            }
            // 게임 시작 신호 처리: "GAME_START"가 포함되면 "GameStart!" 창 표시
            if (msg.find("GAME_START") != std::string::npos) {
                g_gameStarted = true;
                MessageBoxA(NULL, "GameStart!", "Notification", MB_OK);
            }
        }
        else if (recvResult == 0) {
            std::cout << "\n서버 연결 종료" << std::endl;
            break;
        }
        else {
            std::cerr << "\n데이터 수신 실패: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    // 네트워크 초기화 및 서버 연결
    if (!InitNetwork())
        return 1;

    // ReceiverThread 시작
    std::thread recvThread(ReceiverThread, g_clientSocket);

    // 자동으로 로그인 명령 전송 (클라이언트가 접속되면 즉시 로그인)
    std::string loginCmd = "LOGIN TestPlayer 1234\n";
    int sendResult = send(g_clientSocket, loginCmd.c_str(), static_cast<int>(loginCmd.size()), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "로그인 명령 전송 실패: " << WSAGetLastError() << std::endl;
        closesocket(g_clientSocket);
        WSACleanup();
        if (recvThread.joinable())
            recvThread.join();
        return 1;
    }
    std::cout << loginCmd << "명령 전송 완료\n";

    // 로그인 메뉴는 제거하고, 자동으로 g_loggedIn를 true로 처리하도록 설정 (만약 서버가 응답하지 않아도)
    //g_loggedIn = true; // 만약 ReceiverThread에서 LOGIN_OK를 받지 못하면, 이 라인을 주석 해제해 테스트 가능

    // 로그인 성공 후, 서버가 게임 시작 신호(GAME_START)를 보내면 g_gameStarted가 true가 됨.
    // 여기서는 게임이 실행되는 창(DirectX 게임 화면)이 보이도록 Game 객체를 초기화합니다.
    // 만약 게임 시작 전까지 대기하고 싶다면 아래처럼 g_gameStarted를 체크할 수 있습니다.
    while (!g_gameStarted.load()) {
        Sleep(100);
    }

    std::cout << "\n게임 시작 신호 수신. 게임을 시작합니다." << std::endl;

    GWindowInfo.width = 800;
    GWindowInfo.height = 600;
    GWindowInfo.windowed = true;

    unique_ptr<Game> game = make_unique<Game>();
    game->Init(GWindowInfo);

    // 기본 메시지 루프:
    MSG msg;
    HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_CLIENT));
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                break;
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        // 게임 업데이트 (DirectX 렌더링, 입력 처리 등)
        game->Update();
    }

    closesocket(g_clientSocket);
    WSACleanup();
    if (recvThread.joinable())
        recvThread.join();

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    GWindowInfo.hwnd = hWnd;
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
