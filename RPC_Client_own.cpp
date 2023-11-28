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
    int endrequest = 0;
    int game1end = 0, count = 0;

    while (true) {

        while (retry) {

            int clientHand, serverHand;

            std::cout << "가위 바위 보 게임을 시작합니다." << std::endl;
            std::cout << "가위(0), 바위(1), 보(2) 중 하나를 선택하세요: ";
            std::cin >> clientHand;

            send(clientSocket, (char*)&clientHand, sizeof(clientHand), 0);
            recv(clientSocket, (char*)&serverHand, sizeof(serverHand), 0);

            std::cout << "서버 선택한 것: ";
            if (serverHand == 0) {
                std::cout << "가위      ";
            }
            else if (serverHand == 1) {
                std::cout << "바위      ";
            }
            else if (serverHand == 2) {
                std::cout << "보      ";
            }
            std::cout << "클라이언트가 선택한 것: ";
            if (clientHand == 0) {
                std::cout << "가위";
            }
            else if (clientHand == 1) {
                std::cout << "바위";
            }
            else if (clientHand == 2) {
                std::cout << "보";
            }
            std::cout << std::endl;


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

        game1end = 0;
        // 묵찌빠 게임
        while (game1end == 0) {

            int serverChoice;
            recv(clientSocket, (char*)&serverChoice, sizeof(serverChoice), 0);

            char clientChoice;
            std::cout << "묵(0), 찌(1), 빠(2) 중 하나를 선택하세요(승률 (3), 종료 (4)): ";
            std::cin >> clientChoice;

            send(clientSocket, &clientChoice, sizeof(clientChoice), 0);

            if (clientChoice == '4') {
                endrequest = 1;
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

                // 게임 결과가 "승리!" 혹은 "패배!"인 경우 다시 가위바위보 게임으로 돌아감
                if (strcmp(result, "서버 승리! 클라이언트 패배!") == 0 || strcmp(result, "서버 패배! 클라이언트 승리!") == 0) {
                    retry = 1;
                    game1end = 1;
                    count += 1;
                }
            }
        }
        if (endrequest == 1) {
            if (count > 0) {
                double winrate;
                recv(clientSocket, (char*)&winrate, sizeof(winrate), 0);
                std::cout << "서버의 승률 : " << 1.0 - winrate << std::endl;
                std::cout << "클라이언트의 승률 : " << winrate << std::endl;
            }
            std::cout << "프로그램을 종료합니다" << std::endl;
            break;
        }
        else {
            continue;
        }
    }


    // 소켓 닫기
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}