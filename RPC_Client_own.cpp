#include <iostream> // 입출력을 사용하기 위함
#include <winsock2.h>   // 소켓을 사용하기 위한 헤더 파일, API에서 사용하는 상수, 데이터 유형 및 함수 프로토타입 정의
#include <WS2tcpip.h>
#include "protocol.h"   // 프로토콜 헤더 파일

#define ROCK 0
#define SCISSORS 1
#define PAPER 2
#define WIN_REQUEST 3
#define END_REQUEST 4   // 프로그램에서 자주 사용되거나 특별한 트리거를 요구하는 것을 먼저 정의

#pragma comment(lib, "ws2_32.lib")  // 위에서 선언한 헤더파일들을 가져다 쓰기위한 링크, ws2_32.lib 라이브러리 파일을 프로그램에 링크하도록 컴파일러에 알리는 pragma 지시어

int main() {
    WSADATA wsaData;    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {    // Winsock 버전 2.2를 호출하여 설정한다, 이 값이 0이 아니라면
        std::cerr << "WSAStartup failed." << std::endl; // 해당 오류 문장 화면 출력
        return -1;  // 비정상 종료 반환
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);  // 소켓 clientSocket을 IPv4 주소 체계, TCP 프로토콜, SOCK_STREAM 소켓 타입을 사용하여 생성
    if (clientSocket == INVALID_SOCKET) {   // clientSocket이 잘못된 소켓이라면
        std::cerr << "Error creating client socket." << std::endl;  // 해당 오류 문장 화면 출력
        WSACleanup();   // Winsock 종료
        return -1;  // 비정상 종료 반환
    }

    sockaddr_in serverAddress;  // 소켓 주소 serverAddress 선언
    serverAddress.sin_family = AF_INET; // IPv4 주소 체계
    serverAddress.sin_port = htons(9000);   // 9000번 포트

    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {  // IP 주소를 문자열에서 숫자로 바꾼 것이 0보다 작거나 같다면
        std::cerr << "Address conversion failed." << std::endl; // 해당 오류 문장 출력
        closesocket(clientSocket);  // clientSocket 소켓 닫기
        WSACleanup();   // Winsock 종료
        return -1;  // 비정상 종료 반환
    }

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {  // 클라이언트 측에서 서버와의 연결을 요청하고 이에 오류가 발생하면
        std::cerr << "Connection failed." << std::endl; // 해당 오류 문장 출력
        closesocket(clientSocket);  // clientSocket 소켓 닫기
        WSACleanup();   // Winsock 종료
        return -1;  // 비정상 종료 반환
    }

    std::cout << "Connected to the server." << std::endl;   // 서버와 연결되면 해당 문장 출력


    int count = 0;  // count 변수는 게임의 총 경기수를 나타내는 변수 
    int start_request;  // 프로그램 시작 후 게임 시작을 요청한 값을 받는 변수
    
    Packet packet;  // Packet 구조체 형식의  packet 선언

    std::cout << "묵찌빠 게임을 진행하시겠습니까? " << "( 네(1) / 아니오(0) ) : ";
    std::cin >> start_request;  // 클라이언트가 선택한 것을 start_request 변수에 대입

    packet.start_game_request = start_request;  // start_request의 값을 패킷의 start_game_request 필드에 대입
    send(clientSocket, (char*)&packet, sizeof(Packet), 0); // 패킷을 서버로 전송

    while(packet.start_game_request) {   // 패킷의 start_game_request 필드값이 1이면 반복

        while (packet.Game_Choose == 0) {   // 패킷의 Game_Choose 필드값이 0이면 가위바위보 게임 진행

            int clientHand; // 클라이언트가 선택한 값을 받는 변수

            
            std::cout << std::endl << "선공을 위한 가위 바위 보 게임을 시작합니다!!" << std::endl;
            std::cout << "바위(0), 가위(1), 보(2) 중 하나를 선택하세요(승률(3), 종료(4)): ";
            std::cin >> clientHand; // 클라이언트가 선택한 값을 clientHand에 대입

            packet.choice_C = clientHand;   // 클라이언트가 선택한 값을 패킷의 choice_C에 입력
 
            send(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 패킷을 서버로 전송
            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷을 수신

            if (clientHand == END_REQUEST) {    // 클라이언트가 선택한 값이 4라면
                packet.end = 1; // 패킷의 end 필드 값을 1로 변경
                break;      // 반복문을 빠져 나옴
            }
            else if (clientHand == WIN_REQUEST) {   // 클라이언트가 선택한 값이 3이라면
                if (count > 0) {    // 게임을 한 판 이상 진행했을 시
                    
                    recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷을 수신
                    std::cout << std::endl << "<<<<승률 정보 출력>>>>" << std::endl;  
                    std::cout << std::endl << "경기 수 : " << count << " 판" << std::endl;  // 경기 수 출력
                    std::cout << std::endl << "서버가 이긴 경기 수 : " << packet.count - packet.win << " 판" << std::endl;   // 서버가 승리한 횟수 출력
                    std::cout << std::endl << "서버의 승률 : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;  // 서버의 승률 출력
                    std::cout << std::endl << "클라이언트가 이긴 경기 수 : " << packet.win << " 판" << std::endl;   // 클라이언트가 승리한 횟수 출력
                    std::cout << std::endl << "클라이언트의 승률 : " << packet.winrate * 100 << "%" << std::endl;   // 클라이언트의 승률 출력
                    continue;   // 반복문의 처음으로 돌아가서 게임을 다시 진행
                }
                std::cout << std::endl << "아직 한 판도 안하셨습니다." << std::endl;
                continue;   // 반복문의 처음으로 돌아가서 게임을 다시 진행
            }
            
          

            std::cout << "서버가 선택한 것: ";
            if (packet.choice_S == ROCK) {  // 서버가 0을 선택했다면(choice_S 필드의 값이 0이라면)
                std::cout << "바위      ";
            }
            else if (packet.choice_S == SCISSORS) { // 서버가 1을 선택했다면(choice_S 필드의 값이 1이라면)
                std::cout << "가위      ";
            }
            else if (packet.choice_S == PAPER) {    // 서버가 2를 선택했다면(choice_S 필드의 값이 2라면)
                std::cout << "보      ";
            }
            std::cout << "클라이언트가 선택한 것: ";
            if (clientHand == ROCK) {   // 클라이언트가 0을 선택했다면
                std::cout << "바위";
            }
            else if (clientHand == SCISSORS) {  // 클라이언트가 1을 선택했다면
                std::cout << "가위";
            }
            else if (clientHand == PAPER) { // 클라이언트가 2를 선택했다면
                std::cout << "보";
            }
            std::cout << std::endl;

            recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷을 수신
            
            if (packet.Att == 0) {  // 패킷의 Att 필드값이 0이면
                std::cout << "무승부입니다!! 재경기를 진행하세요!" << std::endl;
            }
            else if (packet.Att == 1) { // 패킷의 Att 필드값이 1이라면
                std::cout << "아쉽군요! 서버가 공격입니다!" << std::endl;
                packet.Game_Choose = 1; // 묵찌빠 게임을 진행하기 위해 Game_Choose 필드값을 1로 변경
                break;  // 가위바위 게임 반복문 탈출
            }
            else if (packet.Att == -1) {    // 패킷의 Att 필드값이 -1이라면
                std::cout << "클라이언트가 공격입니다. 기회를 놓치지 마세요!!" << std::endl;
                packet.Game_Choose = 1; //  묵찌빠 게임을 진행하기 위해 Game_Choose 필드값을 1로 변경
                break;  // 가위바위보 게임 반복문 탈출
            }
        }
        
        while (packet.Game_Choose) {    // 패킷의 Game_Choose 값이 1이면 묵찌빠 게임을 진행

            int clientChoice;   // 클라이언트의 선택을 받는 변수 선언

            std::cout << std::endl << "묵찌빠 게임을 시작합니다!!" << std::endl;
            std::cout << "바위(0), 가위(1), 보(2) 중 하나를 선택하세요(승률(3), 종료(4)): ";
            std::cin >> clientChoice;   // 클라이언트 선택을 clientChoice 변수에 입력받음
            packet.choice_C = clientChoice; // clientChoice 변수의 값을 패킷의 choice_C 필드에 대입

            send(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버로 패킷 전송

            if (packet.choice_C == END_REQUEST) {   // 클라이언트의 선택이 4라면
                packet.end = 1;     // end 필드에 1을 대입
                break;  // 반복문을 탈출
            }
            else if (packet.choice_C == WIN_REQUEST) {   // 클라이언트가 선택한 값이 3이라면
                if (count > 0) {    // 게임을 한 판 이상 진행했을 시
                    
                    recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷을 수신
                    std::cout << std::endl << "<<<<승률 정보 출력>>>>" << std::endl;  
                    std::cout << std::endl << "경기 수 : " << count << " 판" << std::endl;  // 경기 수 출력
                    std::cout << std::endl << "서버가 이긴 경기 수 : " << packet.count - packet.win << " 판" << std::endl;   // 서버가 승리한 횟수 출력
                    std::cout << std::endl << "서버의 승률 : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;  // 서버의 승률 출력
                    std::cout << std::endl << "클라이언트가 이긴 경기 수 : " << packet.win << " 판" << std::endl;   // 클라이언트가 승리한 횟수 출력
                    std::cout << std::endl << "클라이언트의 승률 : " << packet.winrate * 100 << "%" << std::endl;   // 클라이언트의 승률 출력
                    continue;   // 반복문의 처음으로 돌아가서 게임을 다시 진행
                }
                std::cout << std::endl << "아직 한 판도 안하셨습니다." << std::endl;
                continue;   // 반복문의 처음으로 돌아가서 게임을 다시 진행
            }
            else {  // 제대로 된 묵찌빠 게임 시작

                send(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버로 패킷 전송
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷 수신
             
                std::cout << "서버가 선택한 것: ";
                if (packet.choice_S == ROCK) {  // 서버가 0을 선택했다면(choice_S 필드 값이 0이라면)
                    std::cout << "바위      ";
                }
                else if (packet.choice_S == SCISSORS) { // 서버가 1을 선택했다면(choice_S 필드 값이 1이라면)
                    std::cout << "가위      ";
                }
                else if (packet.choice_S == PAPER) {    // 서버가 2를 선택했다면(choice_S 필드 값이 2이라면)
                    std::cout << "보      ";
                }

                std::cout << "클라이언트가 선택한 것: ";
                if (packet.choice_C == ROCK) { // 클라이언트가 0을 선택했다면(choice_C 필드 값이 0이라면)
                    std::cout << "바위";
                }
                else if (packet.choice_C == SCISSORS) { // 클라이언트가 1을 선택했다면(choice_C 필드 값이 1이라면)
                    std::cout << "가위";
                }
                else if (packet.choice_C == PAPER) {    // 클라이언트가 2를 선택했다면(choice_C 필드 값이 2라면)
                    std::cout << "보";
                }
                std::cout << std::endl;
                
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷을 수진
                std::cout << std::endl << "결과: " << packet.result_str << std::endl; // 패킷의 result_str 필드 값을 화면에 출력

                // 게임 결과가 "승리!" 혹은 "패배!"인 경우 다시 가위바위보 게임으로 돌아감
                if (strcmp(packet.result_str, "서버 패배! 클라이언트 승리!") == 0 ||
                    strcmp(packet.result_str, "서버 승리! 클라이언트 패배!") == 0) {
                    packet.Game_Choose = 0; // 다시 가위바위보 게임을 진행하기 위해 패킷의 Game_Choose 필드 값을 0으로 변환
                    count++;    // 진행한 경기 수 1 증가
                }
            }
        }

        if (packet.end == 1) {  // 패킷의 end 필드 값이 1이라면
            if (count > 0) {    // 게임을 한 판 이상 진행했다면
                recv(clientSocket, (char*)&packet, sizeof(Packet), 0);  // 서버가 전송한 패킷 수신

                std::cout << std::endl << "<<<<<<<<최종 승률 정보 출력>>>>>>>>" << std::endl;
                std::cout << std::endl << "총 경기 수 : " << count << " 판" << std::endl;    // 총 경기수 출력
                std::cout << std::endl << "서버가 이긴 총 경기 수 : " << packet.count - packet.win << " 판" << std::endl; // 서버가 이긴 횟수 출력
                std::cout << std::endl << "서버의 최종 승률 : " << (1.0 - packet.winrate) * 100 << "%" << std::endl;   // 서버 최종 승률 출력
                std::cout << std::endl << "클라이언트가 이긴 총 경기 수 : " << packet.win << " 판" << std::endl; // 클라이언트가 승리한 횟수 출력
                std::cout << std::endl << "클라이언트의 최종 승률 : " << packet.winrate * 100 << "%" << std::endl;    // 클라이언트 최종 승률 출력
                break;  // 반복문 탈출
            }
            else
                std::cout << std::endl << "한 판은 진행하시고 승률을 요청해주세요." << std::endl;
            break;  // 반복문 탈출
        }
        else {
            continue;   // 가위바위보 게임부터 다시 진행
        }

        closesocket(clientSocket);  // 소켓 clientSocket 닫기
        WSACleanup();   // Winsock 종료

        return 0;   // 프로그램 정상 종료
    }
    std::cout << std::endl << "프로그램을 종료하겠습니다.다음에 또 뵙겠습니다~" << std::endl;
    return 0;   // 프로그램 정상 종료
    
}
