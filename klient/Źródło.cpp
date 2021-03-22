#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include <stdlib.h>
#include <conio.h>
#include<winsock2.h>
#include <windows.h>
#define SERVER_PORT 2000
#define SERVER_ADDR "127.0.0.1"

typedef struct clientData {
	SOCKET sock;
	int board[40][40];
	int scores[50];
};

void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

DWORD WINAPI handleReceive(void * arg) {
	struct clientData * data = (struct clientData*) arg;
	SOCKET si = data->sock;
	char buf[40*40*5];
	printf("\n");
	while (recv(si, buf, 40*40*5, 0) > 0)
	{	
	
		char delim[] = " ";
		char * ptr = strtok(buf, delim);
	
		if (strcmp(ptr, "BOARD") == 0) {

			for (int i = 0; i < 50; i++) {
				data->scores[i] = 0;
			}

			for (int i = 0; i < 40; i++) {
				for (int j = 0; j < 40; j++) {
					ptr = strtok(NULL, delim);
					sscanf(ptr, "%d", &data->board[i][j]);
				}
			}
		
			
			for (int i = 0; i < 40; i++) {
				for (int j = 0; j <40; j++) {
					gotoxy((40-j)*2, i);
					if (data->board[j][i] == -1) {
						printf("\033[0m");
						printf("0 ");
					}
					else if (data->board[j][i] == -2) {
						printf("\033[0;33m");
						printf(". ");
					}
					else {
						//data->board[j][i]-=48;
						printf("\033[0;%dm", 31 + (data->board[j][i]+1) % 10);
						printf("%c ", 'A' + (data->board[j][i] / 10) % 10);
						data->scores[data->board[j][i]]++;
					}
				
					
				}
				printf("\n");
			}
		
			int c = 0;
			for (int i = 0; i < 50; i++) {
				gotoxy(90, 2+c);
		
				if (data->scores[i] != 0) {
					c++;
					printf("\033[0;%dm", 31 + (i + 1) % 10);
					printf("%c ", 'A' + (i / 10) % 10);
					printf("%d", (data->scores[i]));
				}
			}
		}
		if (strcmp(buf, "QUIT") == 0)
		{
			closesocket(si);
			WSACleanup();
			return 0;
		}
		//printf("%s \n", buf);
	};
}

SOCKET createConnection() {
	WSADATA wsas;
	SOCKET s;
	WORD wersja = MAKEWORD(2, 0);
	WSAStartup(wersja, &wsas);
	// POCZ¥TEK TWORZENIA SOCKETU WRAZ Z ADRESEM
	s = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(SERVER_PORT);
	sa.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	// KONIEC TWORZENIA
	int result;

	// £¥CZENIE SOCKETU Z SERVEREM
	result = connect(s, (struct sockaddr FAR *)& sa, sizeof(sa));
	if (result == SOCKET_ERROR) {
		printf("Connection error.");
		return NULL;
	}
	return s;
}
int main() {
	//system("color 70");
	SOCKET socket;
	printf("Attempting to connect");
	while (!(socket = createConnection())) {
		system("cls");
		printf("Trying to connect again in 1 second...");
		Sleep(1000);
		
	}
	system("cls");
	printf("Connection succesful");
	Sleep(1000);
	clientData client_data;
	client_data.sock = socket;
	CreateThread(
		NULL,
		0,
		handleReceive,
		(void *)(&client_data),
		0,
		NULL);
	char buf[80];
	while (1) {
		int z = _getch();
		if (z == 224) {
			z = _getch();
			if (z == 77) { // PRAWO
				strcpy(buf, "LEFT");
			}
			else if (z == 80) {
				strcpy(buf, "DOWN");
			}
			else if (z == 75) {
				strcpy(buf, "RIGHT");
			}
			else if (z == 72) {
				strcpy(buf, "UP");
			}
			send(socket, buf, 80, 0);
		}
		
	}
	return 0;

}