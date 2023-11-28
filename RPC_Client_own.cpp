#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    // ���� �ʱ�ȭ
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }

    // socket()
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error creating client socket." << std::endl;
        WSACleanup();
        return -1;
    }

    // bind()
    SOCKADDR_IN serverAddress;
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

    // ���� ���� �� ����
    int retry = 1;
    int count = 0;
    int game1end = 0;

    while (true) {

        int endrequest = 0, raterequest = 0;

        while (retry == 1) {

            int clientHand, serverHand;

            std::cout << "���� ���� �� ������ �����մϴ�." << std::endl;
            std::cout << "����(0), ����(1), ��(2) �� �ϳ��� �����ϼ���(�·� (3), ���� (4)): ";
            std::cin >> clientHand;

            if (clientHand == 4) {
                endrequest = 1;
                break;
            }
            else if (clientHand == 3) {
                raterequest = 1;
                break;
            }
            else {
                send(clientSocket, (char*)&clientHand, sizeof(clientHand), 0);
                recv(clientSocket, (char*)&serverHand, sizeof(serverHand), 0);

                std::cout << "���� ������ ��: ";
                if (serverHand == 0) {
                    std::cout << "����      ";
                }
                else if (serverHand == 1) {
                    std::cout << "����      ";
                }
                else if (serverHand == 2) {
                    std::cout << "��      ";
                }
                std::cout << "Ŭ���̾�Ʈ�� ������ ��: ";
                if (clientHand == 0) {
                    std::cout << "����";
                }
                else if (clientHand == 1) {
                    std::cout << "����";
                }
                else if (clientHand == 2) {
                    std::cout << "��";
                }
                std::cout << std::endl;


                int Att;
                recv(clientSocket, (char*)&Att, sizeof(Att), 0);

                if (Att == 0) {
                    std::cout << "���º��Դϴ�." << std::endl;
                    retry = 1;
                }
                else if (Att == 1) {
                    std::cout << "������ �̰���ϴ�." << std::endl;
                    retry = 0;
                }
                else {
                    std::cout << "Ŭ���̾�Ʈ�� �̰���ϴ�." << std::endl;
                    retry = 0;
                }
            }
            
        }
        
        game1end = 0;
        // ����� ����
        // (��: 0, ��: 1, ��: 2)
        while (endrequest != 1 && game1end == 0) {

            int serverChoice;
            recv(clientSocket, (char*)&serverChoice, sizeof(serverChoice), 0);

            char clientChoice;
            std::cout << "��(0), ��(1), ��(2) �� �ϳ��� �����ϼ���(�·� (3), ���� (4)): ";
            std::cin >> clientChoice;

            send(clientSocket, &clientChoice, sizeof(clientChoice), 0);

            if (clientChoice == '4') {
                endrequest = 1;
                break;
            }
            else if (clientChoice == '3') {
                raterequest = 1;
                break;
            }
            else {
                std::cout << "������ ������ ��: ";
                if (serverChoice == 0) {
                    std::cout << "��      ";
                }
                else if (serverChoice == 1) {
                    std::cout << "��      ";
                }
                else if (serverChoice == 2) {
                    std::cout << "��      ";
                }

                std::cout << "Ŭ���̾�Ʈ�� ������ ��: ";
                if (clientChoice == '0') {
                    std::cout << "��";
                }
                else if (clientChoice == '1') {
                    std::cout << "��";
                }
                else if (clientChoice == '2') {
                    std::cout << "��";
                }
                std::cout << std::endl;

                char result[50];
                recv(clientSocket, result, sizeof(result), 0);
                std::cout << "���: " << result << std::endl;

                // ���� ����� "�¸�!" Ȥ�� "�й�!"�� ��� �ٽ� ���������� �������� ���ư�
                if (strcmp(result, "�¸�!") == 0 || strcmp(result, "�й�!") == 0) {
                    retry = 1;
                    game1end = 1;
                    count++;
                }
            }
        }
        if (raterequest == 1) {
            std::cout << "�·��� �˷��帮�ڽ��ϴ�" << std::endl;

            double winrate_C = 0, winrate_S = 0;
            recv(clientSocket, (char*)&winrate_C, sizeof(winrate_C), 0);
            
            winrate_C = 1 - winrate_C;
            std::cout << "������ �·�: " << winrate_S << std::endl;
            std::cout << "Ŭ���̾�Ʈ�� �·�: " << winrate_C << std::endl;
            

        }
        else if (endrequest == 1) {
            if (count > 0) {
                std::cout << "�����·��� �˷��帮�ڽ��ϴ�" << std::endl;

                double winrate_C = 0, winrate_S = 0;
                recv(clientSocket, (char*)&winrate_C, sizeof(winrate_C), 0);

                winrate_C = 1 - winrate_C;
                std::cout << "������ �·�: " << winrate_S << std::endl;
                std::cout << "Ŭ���̾�Ʈ�� �·�: " << winrate_C << std::endl;

            }
          
            std::cout << "���α׷��� �����մϴ�" << std::endl;
            break;
        }
        else {
            continue;
        }
    }


    // ���� �ݱ�
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
