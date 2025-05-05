#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "Game.h"
#include "protocol.h"     
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <limits>
#include <sstream>
#include <iomanip>

#include "Scene.h"
#include "SceneManager.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_LOADSTRING 100

WindowInfo GWindowInfo;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

SOCKET g_clientSocket = INVALID_SOCKET;

// 로그인 상태 플래그 (테스트용: 로그인 후 설정)
std::atomic<bool> g_loggedIn(false);

std::atomic<bool> g_gameStarted(false);

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

static uint32_t g_localPlayerId = 0;

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
    if (InetPtonA(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1) {
        std::wcerr << L"IP 주소 변환 실패" << std::endl;
        closesocket(g_clientSocket);
        WSACleanup();
        return false;
    }
    if (connect(g_clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::wcerr << L"서버 연결 실패: " << WSAGetLastError() << std::endl;
        closesocket(g_clientSocket);
        WSACleanup();
        return false;
    }
    std::wcout << L"네트워크 초기화 및 서버 연결 성공" << std::endl;
    return true;
}

void ReceiverThread(SOCKET clientSocket) {
    char buffer[8192];
    while (true) {
        int recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (recvResult > 0) {
            if (recvResult >= 2) {
                unsigned char pktSize = buffer[0];
                char pktType = buffer[1];
                std::ostringstream oss;
                oss << "[서버 응답] Packet: Size=" << static_cast<int>(pktSize)
                    << ", Type=" << static_cast<int>(pktType)
                    << ", Raw Data: ";
                for (int i = 0; i < recvResult; i++) {
                    oss << std::hex << std::setw(2) << std::setfill('0')
                        << (static_cast<int>(static_cast<unsigned char>(buffer[i]))) << " ";
                }
                std::cout << "\n" << oss.str() << std::endl;

                switch (pktType) {
                case S2C_P_PLAYER_INFO: {
                    sc_packet_login_ok* pLoginOk = reinterpret_cast<sc_packet_login_ok*>(buffer);
                        if (!g_loggedIn) {
                        g_loggedIn = true;
                        g_localPlayerId = static_cast<uint32_t>(pLoginOk->playerId);
                    }
                        if (static_cast<uint32_t>(pLoginOk->playerId) != g_localPlayerId) {
                        GET_SINGLE(SceneManager)->GetActiveScene()->AddPlayer(pLoginOk);
                    }
                    break;
                }
                case S2C_P_MOVE: {
					sc_packet_move* pMove = reinterpret_cast<sc_packet_move*>(buffer);
					GET_SINGLE(SceneManager)->GetActiveScene()->MovePlayer(pMove);
                    
                    break;
                }
                case S2C_P_ATTACK: {
                    MessageBoxA(NULL, "공격 이벤트 수신", "Debug - Attack", MB_OK);
                    break;
                }
                case S2C_P_PLAYER_LEAVE: {
                    sc_packet_player_leave* pLeave = reinterpret_cast<sc_packet_player_leave*>(buffer);
                    GET_SINGLE(SceneManager)
                        ->GetActiveScene()
                        ->RemovePlayer(pLeave);
                    break;
                }
                case S2C_P_GAME_START: {
                    GET_SINGLE(SceneManager)
                         ->GetActiveScene()
                         ->ClearPlayers();
                         g_gameStarted = true;
                    break;
                }
                default:
                    std::cout << "[클라이언트] 정의되지 않은 패킷 타입: " << static_cast<int>(pktType) << std::endl;
                    break;
                }
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

    if (!InitNetwork())
        return 1;

    GWindowInfo.width = 1280;
    GWindowInfo.height = 800;
    GWindowInfo.windowed = true;
    GWindowInfo.sock = g_clientSocket;

    std::unique_ptr<Game> game = std::make_unique<Game>();
    game->Init(GWindowInfo);

    std::thread recvThread(ReceiverThread, g_clientSocket);
    // 자동 로그인: cs_packet_login 패킷 전송
    cs_packet_login loginPacket;
    loginPacket.size = sizeof(cs_packet_login);
    loginPacket.type = C2S_P_LOGIN;
    int sendResult = send(g_clientSocket, reinterpret_cast<char*>(&loginPacket), sizeof(loginPacket), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "로그인 패킷 전송 실패: " << WSAGetLastError() << std::endl;
        closesocket(g_clientSocket);
        WSACleanup();
        if (recvThread.joinable())
            recvThread.join();
        return 1;
    }
    std::cout << "자동 로그인 패킷 전송 완료: size=" << (int)loginPacket.size << ", type=" << (int)loginPacket.type << std::endl;
   
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
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   if (GetForegroundWindow() != hWnd)
   {
       if (IsIconic(hWnd))
           ShowWindow(hWnd, SW_RESTORE);

       SetForegroundWindow(hWnd);
       SetFocus(hWnd);
   }

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
