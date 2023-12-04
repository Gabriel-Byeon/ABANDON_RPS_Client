#include <iostream> // ������� ����ϱ� ����
#include <winsock2.h>   // ������ ����ϱ� ���� ��� ����, API���� ����ϴ� ���, ������ ���� �� �Լ� ������Ÿ�� ����
#include <WS2tcpip.h>
#include "protocol.h"   // �������� ��� ����

#define ROCK 0
#define SCISSORS 1
#define PAPER 2
#define WIN_REQUEST 3
#define END_REQUEST 4   // ���α׷����� ���� ���ǰų� Ư���� Ʈ���Ÿ� �䱸�ϴ� ���� ���� ����

#pragma comment(lib, "ws2_32.lib")  // ������ ������ ������ϵ��� ������ �������� ��ũ, ws2_32.lib ���̺귯�� ������ ���α׷��� ��ũ�ϵ��� �����Ϸ��� �˸��� pragma ���þ�

int main() {
    WSADATA wsaData;    // Winsock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {    // Winsock ���� 2.2�� ȣ���Ͽ� �����Ѵ�, �� ���� 0�� �ƴ϶��
        std::cerr << "WSAStartup failed." << std::endl; // �ش� ���� ���� ȭ�� ���
        return -1;  // ������ ���� ��ȯ
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);  // ���� clientSocket�� IPv4 �ּ� ü��, TCP ��������, SOCK_STREAM ���� Ÿ���� ����Ͽ� ����
    if (clientSocket == INVALID_SOCKET) {   // clientSocket�� �߸��� �����̶��
        std::cerr << "Error creating client socket." << std::endl;  // �ش� ���� ���� ȭ�� ���
        WSACleanup();   // Winsock ����
        return -1;  // ������ ���� ��ȯ
    }

    sockaddr_in serverAddress;  // ���� �ּ� serverAddress ����
    serverAddress.sin_family = AF_INET; // IPv4 �ּ� ü��
    serverAddress.sin_port = htons(9000);   // 9000�� ��Ʈ

    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {  // IP �ּҸ� ���ڿ����� ���ڷ� �ٲ� ���� 0���� �۰ų� ���ٸ�
        std::cerr << "Address conversion failed." << std::endl; // �ش� ���� ���� ���
        closesocket(clientSocket);  // clientSocket ���� �ݱ�
        WSACleanup();   // Winsock ����
        return -1;  // ������ ���� ��ȯ
    }

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {  // Ŭ���̾�Ʈ ������ �������� ������ ��û�ϰ� �̿� ������ �߻��ϸ�
        std::cerr << "Connection failed." << std::endl; // �ش� ���� ���� ���
        closesocket(clientSocket);  // clientSocket ���� �ݱ�
        WSACleanup();   // Winsock ����
        return -1;  // ������ ���� ��ȯ
    }

    std::cout << "Connected to the server." << std::endl;   // ������ ����Ǹ� �ش� ���� ���


    int count = 0;  // count ������ ������ �� ������ ��Ÿ���� ���� 
    int start_request;  // ���α׷� ���� �� ���� ������ ��û�� ���� �޴� ����
    
    Packet packet;  // Packet ����ü ������  packet ����

    std::cout << "����� ������ �����Ͻðڽ��ϱ�? " << "( ��(1) / �ƴϿ�(0) ) : ";
    std::cin >> start_request;  // Ŭ���̾�Ʈ�� ������ ���� start_request ������ ����

    packet.start_game_request = start_request;  // start_request�� ���� ��Ŷ�� start_game_request �ʵ忡 ����
    send(clientSocket, (char*)&packet, sizeof(Packet), 0); // ��Ŷ�� ������ ����

    while(packet.start_game_request) {   // ��Ŷ�� start_game_request �ʵ尪�� 1�̸� �ݺ�

        while (packet.Game_Choose == 0) {   // ��Ŷ�� Game_Choose �ʵ尪�� 0�̸� ���������� ���� ����

            int clientHand; // Ŭ���̾�Ʈ�� ������ ���� �޴� ����

            
            std::cout << std::endl << "������ ���� ���� ���� �� ������ �����մϴ�!!" << std::endl;
            std::cout << "����(0), ����(1), ��(2) �� �ϳ��� �����ϼ���(�·�(3), ����(4)): ";
            std::cin >> clientHand; // Ŭ���̾�Ʈ�� ������ ���� clientHand�� ����

            packet.choice_C = clientHand;   // Ŭ���̾�Ʈ�� ������ ���� ��Ŷ�� choice_C�� �Է�
 
            send(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ��Ŷ�� ������ ����
            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ�� ����

            if (clientHand == END_REQUEST) {    // Ŭ���̾�Ʈ�� ������ ���� 4���
                packet.end = 1; // ��Ŷ�� end �ʵ� ���� 1�� ����
                break;      // �ݺ����� ���� ����
            }
            else if (clientHand == WIN_REQUEST) {   // Ŭ���̾�Ʈ�� ������ ���� 3�̶��
                if (count > 0) {    // ������ �� �� �̻� �������� ��
                    
                    recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ�� ����
                    std::cout << std::endl << "<<<<�·� ���� ���>>>>" << std::endl;  
                    std::cout << std::endl << "��� �� : " << count << " ��" << std::endl;  // ��� �� ���
                    std::cout << std::endl << "������ �̱� ��� �� : " << packet.count - packet.win << " ��" << std::endl;   // ������ �¸��� Ƚ�� ���
                    std::cout << std::endl << "������ �·� : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;  // ������ �·� ���
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �̱� ��� �� : " << packet.win << " ��" << std::endl;   // Ŭ���̾�Ʈ�� �¸��� Ƚ�� ���
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �·� : " << packet.winrate * 100 << "%" << std::endl;   // Ŭ���̾�Ʈ�� �·� ���
                    continue;   // �ݺ����� ó������ ���ư��� ������ �ٽ� ����
                }
                std::cout << std::endl << "���� �� �ǵ� ���ϼ̽��ϴ�." << std::endl;
                continue;   // �ݺ����� ó������ ���ư��� ������ �ٽ� ����
            }
            
          

            std::cout << "������ ������ ��: ";
            if (packet.choice_S == ROCK) {  // ������ 0�� �����ߴٸ�(choice_S �ʵ��� ���� 0�̶��)
                std::cout << "����      ";
            }
            else if (packet.choice_S == SCISSORS) { // ������ 1�� �����ߴٸ�(choice_S �ʵ��� ���� 1�̶��)
                std::cout << "����      ";
            }
            else if (packet.choice_S == PAPER) {    // ������ 2�� �����ߴٸ�(choice_S �ʵ��� ���� 2���)
                std::cout << "��      ";
            }
            std::cout << "Ŭ���̾�Ʈ�� ������ ��: ";
            if (clientHand == ROCK) {   // Ŭ���̾�Ʈ�� 0�� �����ߴٸ�
                std::cout << "����";
            }
            else if (clientHand == SCISSORS) {  // Ŭ���̾�Ʈ�� 1�� �����ߴٸ�
                std::cout << "����";
            }
            else if (clientHand == PAPER) { // Ŭ���̾�Ʈ�� 2�� �����ߴٸ�
                std::cout << "��";
            }
            std::cout << std::endl;

            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ�� ����
            
            if (packet.Att == 0) {  // ��Ŷ�� Att �ʵ尪�� 0�̸�
                std::cout << "���º��Դϴ�!! ���⸦ �����ϼ���!" << std::endl;
            }
            else if (packet.Att == 1) { // ��Ŷ�� Att �ʵ尪�� 1�̶��
                std::cout << "�ƽ�����! ������ �����Դϴ�!" << std::endl;
                packet.Game_Choose = 1; // ����� ������ �����ϱ� ���� Game_Choose �ʵ尪�� 1�� ����
                break;  // �������� ���� �ݺ��� Ż��
            }
            else if (packet.Att == -1) {    // ��Ŷ�� Att �ʵ尪�� -1�̶��
                std::cout << "Ŭ���̾�Ʈ�� �����Դϴ�. ��ȸ�� ��ġ�� ������!!" << std::endl;
                packet.Game_Choose = 1; //  ����� ������ �����ϱ� ���� Game_Choose �ʵ尪�� 1�� ����
                break;  // ���������� ���� �ݺ��� Ż��
            }
        }
        
        while (packet.Game_Choose) {    // ��Ŷ�� Game_Choose ���� 1�̸� ����� ������ ����

            int clientChoice;   // Ŭ���̾�Ʈ�� ������ �޴� ���� ����

            std::cout << std::endl << "����� ������ �����մϴ�!!" << std::endl;
            std::cout << "����(0), ����(1), ��(2) �� �ϳ��� �����ϼ���(�·�(3), ����(4)): ";
            std::cin >> clientChoice;   // Ŭ���̾�Ʈ ������ clientChoice ������ �Է¹���
            packet.choice_C = clientChoice; // clientChoice ������ ���� ��Ŷ�� choice_C �ʵ忡 ����

            send(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ��Ŷ ����

            if (packet.choice_C == END_REQUEST) {   // Ŭ���̾�Ʈ�� ������ 4���
                packet.end = 1;     // end �ʵ忡 1�� ����
                break;  // �ݺ����� Ż��
            }
            else if (packet.choice_C == WIN_REQUEST) {   // Ŭ���̾�Ʈ�� ������ ���� 3�̶��
                if (count > 0) {    // ������ �� �� �̻� �������� ��
                    
                    recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ�� ����
                    std::cout << std::endl << "<<<<�·� ���� ���>>>>" << std::endl;  
                    std::cout << std::endl << "��� �� : " << count << " ��" << std::endl;  // ��� �� ���
                    std::cout << std::endl << "������ �̱� ��� �� : " << packet.count - packet.win << " ��" << std::endl;   // ������ �¸��� Ƚ�� ���
                    std::cout << std::endl << "������ �·� : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;  // ������ �·� ���
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �̱� ��� �� : " << packet.win << " ��" << std::endl;   // Ŭ���̾�Ʈ�� �¸��� Ƚ�� ���
                    std::cout << std::endl << "Ŭ���̾�Ʈ�� �·� : " << packet.winrate * 100 << "%" << std::endl;   // Ŭ���̾�Ʈ�� �·� ���
                    continue;   // �ݺ����� ó������ ���ư��� ������ �ٽ� ����
                }
                std::cout << std::endl << "���� �� �ǵ� ���ϼ̽��ϴ�." << std::endl;
                continue;   // �ݺ����� ó������ ���ư��� ������ �ٽ� ����
            }
            else {  // ����� �� ����� ���� ����

                send(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ��Ŷ ����
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ ����
             
                std::cout << "������ ������ ��: ";
                if (packet.choice_S == ROCK) {  // ������ 0�� �����ߴٸ�(choice_S �ʵ� ���� 0�̶��)
                    std::cout << "����      ";
                }
                else if (packet.choice_S == SCISSORS) { // ������ 1�� �����ߴٸ�(choice_S �ʵ� ���� 1�̶��)
                    std::cout << "����      ";
                }
                else if (packet.choice_S == PAPER) {    // ������ 2�� �����ߴٸ�(choice_S �ʵ� ���� 2�̶��)
                    std::cout << "��      ";
                }

                std::cout << "Ŭ���̾�Ʈ�� ������ ��: ";
                if (packet.choice_C == ROCK) { // Ŭ���̾�Ʈ�� 0�� �����ߴٸ�(choice_C �ʵ� ���� 0�̶��)
                    std::cout << "����";
                }
                else if (packet.choice_C == SCISSORS) { // Ŭ���̾�Ʈ�� 1�� �����ߴٸ�(choice_C �ʵ� ���� 1�̶��)
                    std::cout << "����";
                }
                else if (packet.choice_C == PAPER) {    // Ŭ���̾�Ʈ�� 2�� �����ߴٸ�(choice_C �ʵ� ���� 2���)
                    std::cout << "��";
                }
                std::cout << std::endl;
                
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ�� ����
                std::cout << std::endl << "���: " << packet.result_str << std::endl; // ��Ŷ�� result_str �ʵ� ���� ȭ�鿡 ���

                // ���� ����� "�¸�!" Ȥ�� "�й�!"�� ��� �ٽ� ���������� �������� ���ư�
                if (strcmp(packet.result_str, "���� �й�! Ŭ���̾�Ʈ �¸�!") == 0 ||
                    strcmp(packet.result_str, "���� �¸�! Ŭ���̾�Ʈ �й�!") == 0) {
                    packet.Game_Choose = 0; // �ٽ� ���������� ������ �����ϱ� ���� ��Ŷ�� Game_Choose �ʵ� ���� 0���� ��ȯ
                    count++;    // ������ ��� �� 1 ����
                }
            }
        }

        if (packet.end == 1) {  // ��Ŷ�� end �ʵ� ���� 1�̶��
            if (count > 0) {    // ������ �� �� �̻� �����ߴٸ�
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // ������ ������ ��Ŷ ����

                std::cout << std::endl << "<<<<<<<<���� �·� ���� ���>>>>>>>>" << std::endl;
                std::cout << std::endl << "�� ��� �� : " << count << " ��" << std::endl;    // �� ���� ���
                std::cout << std::endl << "������ �̱� �� ��� �� : " << packet.count - packet.win << " ��" << std::endl; // ������ �̱� Ƚ�� ���
                std::cout << std::endl << "������ ���� �·� : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;   // ���� ���� �·� ���
                std::cout << std::endl << "Ŭ���̾�Ʈ�� �̱� �� ��� �� : " << packet.win << " ��" << std::endl; // Ŭ���̾�Ʈ�� �¸��� Ƚ�� ���
                std::cout << std::endl << "Ŭ���̾�Ʈ�� ���� �·� : " << packet.winrate * 100 << "%" << std::endl;    // Ŭ���̾�Ʈ ���� �·� ���
                break;  // �ݺ��� Ż��
            }
            else
                std::cout << std::endl << "�� ���� �����Ͻð� �·��� ��û���ּ���." << std::endl;
            break;  // �ݺ��� Ż��
        }
        else {
            continue;   // ���������� ���Ӻ��� �ٽ� ����
        }

        closesocket(clientSocket);  // ���� clientSocket �ݱ�
        WSACleanup();   // Winsock ����

        return 0;   // ���α׷� ���� ����
    }
    std::cout << std::endl << "���α׷��� �����ϰڽ��ϴ�.������ �� �˰ڽ��ϴ�~" << std::endl;
    return 0;   // ���α׷� ���� ����
    
}
