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

    std::cout << "����� ���� ���� ��Ű���? " << "���� ��밡 ������ ��û�߽��ϴ�! (������ ���Ѵ�!(1) / ��������(0)) : ";
    std::cin >> start_request;
    std::cout << std::endl << "������ ���ϼ̱���! ������ ���ϴ�!" << std::endl;

    packet.start_game_request = start_request;
    send(clientSocket, (char*)&packet, sizeof(Packet), 0);

    while(packet.start_game_request) {

        while (packet.Game_Choose == 0) {

            int clientHand;

            
            std::cout << std::endl << "������ ���� ���� ���� �� ������ �����մϴ�!!" << std::endl;
            std::cout << "����(0), ����(1), ��(2) �� �ϳ��� �����ϼ���(�·�(3), ����(4)): ";
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
                    std::cout << std::endl << "<<<<�·� ���� ���>>>>" << std::endl;
                    std::cout << std::endl << "��� �� : " << count << " ��" << std::endl;
                    std::cout << std::endl << "������ �̱� ��� �� : " << packet.count - packet.win << " ��" << std::endl;
                    std::cout << std::endl << "������ �·� : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �̱� ��� �� : " << packet.win << " ��" << std::endl;
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �·� : " << packet.winrate * 100 << "%" << std::endl;
                    continue;
                }
                std::cout << std::endl << "���� �� �ǵ� ���ϼ̽��ϴ�." << std::endl;
                continue;
            }
            
          

            std::cout << "������ ������ ��: ";
            if (packet.choice_S == ROCK) {
                std::cout << "����      ";
            }
            else if (packet.choice_S == SCISSORS) {
                std::cout << "����      ";
            }
            else if (packet.choice_S == PAPER) {
                std::cout << "��      ";
            }
            std::cout << "Ŭ���̾�Ʈ�� ������ ��: ";
            if (clientHand == ROCK) {
                std::cout << "����";
            }
            else if (clientHand == SCISSORS) {
                std::cout << "����";
            }
            else if (clientHand == PAPER) {
                std::cout << "��";
            }
            std::cout << std::endl;

            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
            
            if (packet.Att == 0) {
                std::cout << "���º��Դϴ�!! ���⸦ �����ϼ���!" << std::endl;
            }
            else if (packet.Att == 1) {
                std::cout << "�ƽ�����! ������ �����Դϴ�!" << std::endl;
                packet.Game_Choose = 1;
                break;
            }
            else if (packet.Att == -1) {
                std::cout << "Ŭ���̾�Ʈ�� �����Դϴ�. ��ȸ�� ��ġ�� ������!!" << std::endl;
                packet.Game_Choose = 1;
                break;
            }
        }
        
        while (packet.Game_Choose) {

            int clientChoice;

            std::cout << std::endl << "����� ������ �����մϴ�!!" << std::endl;
            std::cout << "����(0), ����(1), ��(2) �� �ϳ��� �����ϼ���(�·�(3), ����(4)): ";
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

                    std::cout << std::endl << "<<<<�·� ���� ���>>>>" << std::endl;
                    std::cout << std::endl << "��� �� : " << count << " ��" << std::endl;
                    std::cout << std::endl << "������ �̱� ��� �� : " << packet.count - packet.win << " ��" << std::endl;
                    std::cout << std::endl << "������ �·� : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �̱� ��� �� : " << packet.win << " ��" << std::endl;
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �·� : " << packet.winrate * 100 << "%" << std::endl;
                    continue;
                }
                std::cout << std::endl << "�� ���� �����Ͻð� �·��� ��û���ּ���." << std::endl;
                continue;
            }
            else {

                send(clientSocket, (char*)&packet, sizeof(Packet), 0);
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
             
                std::cout << "������ ������ ��: ";
                if (packet.choice_S == ROCK) {
                    std::cout << "����      ";
                }
                else if (packet.choice_S == SCISSORS) {
                    std::cout << "����      ";
                }
                else if (packet.choice_S == PAPER) {
                    std::cout << "��      ";
                }

                std::cout << "Ŭ���̾�Ʈ�� ������ ��: ";
                if (packet.choice_C == ROCK) {
                    std::cout << "����";
                }
                else if (packet.choice_C == SCISSORS) {
                    std::cout << "����";
                }
                else if (packet.choice_C == PAPER) {
                    std::cout << "��";
                }
                std::cout << std::endl;
                
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);
                std::cout << std::endl << "���: " << packet.result_str << std::endl;

                // ���� ����� "�¸�!" Ȥ�� "�й�!"�� ��� �ٽ� ���������� �������� ���ư�
                if (strcmp(packet.result_str, "���� �й�! Ŭ���̾�Ʈ �¸�!") == 0 ||
                    strcmp(packet.result_str, "���� �¸�! Ŭ���̾�Ʈ �й�!") == 0) {
                    packet.Game_Choose = 0;
                    count++;
                }
            }
        }

        if (packet.end == 1) {
            if (count > 0) {
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);

                std::cout << std::endl << "<<<<<<<<���� �·� ���� ���>>>>>>>>" << std::endl;
                std::cout << std::endl << "�� ��� �� : " << count << " ��" << std::endl;
                std::cout << std::endl << "������ �̱� �� ��� �� : " << packet.count - packet.win << " ��" << std::endl;
                std::cout << std::endl << "������ ���� �·� : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;
                std::cout << std::endl << "Ŭ���̾�Ʈ�� �̱� �� ��� �� : " << packet.win << " ��" << std::endl;
                std::cout << std::endl << "Ŭ���̾�Ʈ�� ���� �·� : " << packet.winrate * 100 << "%" << std::endl;
                break;
            }
            else
                std::cout << std::endl << "�� ���� �����Ͻð� �·��� ��û���ּ���." << std::endl;
            break;
        }
        else {
            continue;
        }

        closesocket(clientSocket);
        WSACleanup();

        return 0;
    }
    std::cout << std::endl << "���α׷��� �����ϰڽ��ϴ�.������ �� �˰ڽ��ϴ�~" << std::endl;
    return 0;
    
}
