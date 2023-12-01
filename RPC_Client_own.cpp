#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "protocol.h"

#define ROCK 0
#define SCISSORS 1
#define PAPER 2
#define WIN_REQUEST 3
#define END_REQUEST 4

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

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connection failed." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to the server." << std::endl;


    int count = 0;
    int start_request;
    
    Packet packet;

    std::cout << "묵찌빠 게임을 하시겠습니까?(예(1) / 아니오(0)) : ";
    std::cin >> start_request;
    packet.start_game_request = start_request;
    send(clientSocket, (char*)&packet, sizeof(Packet), 0);

    while(packet.start_game_request) {

        while (packet.Game_Choose == 0) {

            int clientHand;

            std::cout << std::endl << "가위 바위 보 게임을 시작합니다." << std::endl;
            std::cout << "바위(0), 가위(1), 보(2) 중 하나를 선택하세요(승률(3), 종료(4)): ";
            std::cin >> clientHand;

            // Packet packet;
            packet.choice_C = clientHand;
 
            send(clientSocket, (char*)&packet, sizeof(Packet), 0);
            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);

            if (clientHand == END_REQUEST) {
                packet.end = 1;
                break;
            }
            else if (clientHand == WIN_REQUEST) {
                if (count > 0) {
                    
                    recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
                    std::cout << std::endl << "서버의 승률 : " << 1.0 - packet.winrate << std::endl;
                    std::cout << std::endl << "클라이언트의 승률 : " << packet.winrate << std::endl;
                    continue;
                }
                std::cout << std::endl << "아직 한 판도 안하셨습니다." << std::endl;
                continue;
            }
            
          

            std::cout << "서버 선택한 것: ";
            if (packet.choice_S == ROCK) {
                std::cout << "바위      ";
            }
            else if (packet.choice_S == SCISSORS) {
                std::cout << "가위      ";
            }
            else if (packet.choice_S == PAPER) {
                std::cout << "보      ";
            }
            std::cout << "클라이언트가 선택한 것: ";
            if (clientHand == ROCK) {
                std::cout << "바위";
            }
            else if (clientHand == SCISSORS) {
                std::cout << "가위";
            }
            else if (clientHand == PAPER) {
                std::cout << "보";
            }
            std::cout << std::endl;

            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
            
            if (packet.Att == 0) {
                std::cout << "무승부입니다." << std::endl;
            }
            else if (packet.Att == 1) {
                std::cout << "서버가 공격입니다." << std::endl;
                packet.Game_Choose = 1;
                break;
            }
            else if (packet.Att == -1) {
                std::cout << "클라이언트가 공격입니다." << std::endl;
                packet.Game_Choose = 1;
                break;
            }
        }
        
        while (packet.Game_Choose) {

            int clientChoice;

            std::cout << std::endl << "묵찌빠 게임을 시작합니다." << std::endl;
            std::cout << "바위(0), 가위(1), 보(2) 중 하나를 선택하세요(승률(3), 종료(4)): ";
            std::cin >> clientChoice;
            packet.choice_C = clientChoice;

            send(clientSocket, (char*)&packet, sizeof(Packet), 0);

            if (packet.choice_C == END_REQUEST) {
                packet.end = 1;
                break;
            }
            else if (packet.choice_C == WIN_REQUEST) {
                if (count > 0) {
                    recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
                    std::cout << std::endl << "서버의 승률 : " << 1.0 - packet.winrate << std::endl;
                    std::cout << std::endl << "클라이언트의 승률 : " << packet.winrate << std::endl;
                    continue;
                }
                std::cout << std::endl << "아직 한 판도 안하셨습니다." << std::endl;
                continue;
            }
            else {

                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
                send(clientSocket, (char*)&packet, sizeof(Packet), 0);
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
             
                std::cout << "서버가 선택한 것: ";
                if (packet.choice_S == ROCK) {
                    std::cout << "바위      ";
                }
                else if (packet.choice_S == SCISSORS) {
                    std::cout << "가위      ";
                }
                else if (packet.choice_S == PAPER) {
                    std::cout << "보      ";
                }

                std::cout << "클라이언트가 선택한 것: ";
                if (packet.choice_C == ROCK) {
                    std::cout << "바위";
                }
                else if (packet.choice_C == SCISSORS) {
                    std::cout << "가위";
                }
                else if (packet.choice_C == PAPER) {
                    std::cout << "보";
                }
                std::cout << std::endl;
                
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
                std::cout << std::endl << "결과: " << packet.result_str << std::endl;

                // 게임 결과가 "승리!" 혹은 "패배!"인 경우 다시 가위바위보 게임으로 돌아감
                if (strcmp(packet.result_str, "서버 패배! 클라이언트 승리!") == 0 ||
                    strcmp(packet.result_str, "서버 승리! 클라이언트 패배!") == 0) {
                    packet.Game_Choose = 0;
                    count++;
                }
            }
        }

        if (packet.end == 1) {
            if (count > 0) {
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
                std::cout << std::endl << "<<<<<<<<최종 승률>>>>>>>>" << std::endl;
                std::cout << "서버의 승률 : " << 1.0 - packet.winrate << std::endl;
                std::cout << "클라이언트의 승률 : " << packet.winrate << std::endl;
            }
            else
                std::cout << std::endl << "한 판도 안하셔도 승률을 낼 수 없습니다." << std::endl;
            break;
        }
        else {
            continue;
        }

        closesocket(clientSocket);
        WSACleanup();

        return 0;
    }
    std::cout << std::endl << "프로그램을 종료하겠습니다." << std::endl;
    return 0;
    
}
