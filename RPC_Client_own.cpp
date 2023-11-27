#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating client socket." << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);

    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Address conversion failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to the server." << std::endl;

    // 가위 바위 보 게임
    int retry = 1;

    while (retry == 1) {
        int clientHand;
        std::cout << "가위 바위 보 게임을 시작합니다." << std::endl;
        std::cout << "가위(0), 바위(1), 보(2) 중 하나를 선택하세요(승률 (3), 종료 (4)): ";
        std::cin >> clientHand;

        send(clientSocket, (char*)&clientHand, sizeof(clientHand), 0);

        int Att;
        recv(clientSocket, (char*)&Att, sizeof(Att), 0);

        if (Att == 0) {
            std::cout << "무승부입니다." << std::endl;
            retry = 1;
        }
        else if (Att == 1) {
            std::cout << "서버가 이겼습니다." << std::endl;
            retry = 0;
        }
        else {
            std::cout << "클라이언트가 이겼습니다." << std::endl;
            retry = 0;
        }
    }


    // 묵찌빠 게임
    while (true) {
        int serverChoice;
        recv(clientSocket, (char*)&serverChoice, sizeof(serverChoice), 0);

        char clientChoice;
        std::cout << "묵(0), 찌(1), 빠(2) 중 하나를 선택하세요(승률 (3), 종료 (4)): ";
        std::cin >> clientChoice;

        send(clientSocket, &clientChoice, sizeof(clientChoice), 0);

        if (clientChoice == '4') {
            break;
        }
        else {
            std::cout << "서버가 선택한 것: ";
            if (serverChoice == 0) {
                std::cout << "묵      ";
            }
            else if (serverChoice == 1) {
                std::cout << "찌      ";
            }
            else if (serverChoice == 2) {
                std::cout << "빠      ";
            }

            std::cout << "클라이언트가 선택한 것: ";
            if (clientChoice == '0') {
                std::cout << "묵";
            }
            else if (clientChoice == '1') {
                std::cout << "찌";
            }
            else if (clientChoice == '2') {
                std::cout << "빠";
            }
            std::cout << std::endl;


            char result[50];
            recv(clientSocket, result, sizeof(result), 0);
            std::cout << "결과: " << result << std::endl;
        }
        
        /*char playAgain;
        std::cout << "게임을 계속하시겠습니까? (y/n): ";
        std::cin >> playAgain;

        send(clientSocket, &playAgain, sizeof(playAgain), 0);

        if (playAgain != 'y') {
            break;
        }*/
    }

    // 소켓 닫기
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
