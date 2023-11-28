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

    // ���� ���� �� ����
    int retry = 1;
    int endrequest = 0;
    int game1end = 0, count = 0;

    while (true) {

        while (retry) {

            int clientHand, serverHand;

            std::cout << "���� ���� �� ������ �����մϴ�." << std::endl;
            std::cout << "����(0), ����(1), ��(2) �� �ϳ��� �����ϼ���: ";
            std::cin >> clientHand;

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

        game1end = 0;
        // ����� ����
        while (game1end == 0) {

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
                if (strcmp(result, "���� �¸�! Ŭ���̾�Ʈ �й�!") == 0 || strcmp(result, "���� �й�! Ŭ���̾�Ʈ �¸�!") == 0) {
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
                std::cout << "������ �·� : " << 1.0 - winrate << std::endl;
                std::cout << "Ŭ���̾�Ʈ�� �·� : " << winrate << std::endl;
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