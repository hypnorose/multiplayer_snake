#ifndef UNICODE
#define UNICODE
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
#define size 10
struct dane_dla_watku
{
	int port;
	char* adresIP;
};
DWORD WINAPI funkcja_watku(void* argumenty);
HANDLE watek(int port);
struct dane_dla_watku* SpawnParams(int port) {
	struct dane_dla_watku* params = (struct dane_dla_watku*)malloc(sizeof(struct dane_dla_watku));
	params->port = port;
	return params;
}
DWORD WINAPI funkcja_watku(void* argumenty)
{
	struct dane_dla_watku* moje_dane = (struct dane_dla_watku*) argumenty;
	int port = moje_dane->port;
	WSADATA wsas;
	int result;
	WORD wersja;
	wersja = MAKEWORD(1, 1);
	result = WSAStartup(wersja, &wsas);
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sa;
	memset((void*)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	result = bind(s, (struct sockaddr FAR*) & sa, sizeof(sa));
	result = listen(s, 5);

	SOCKET si;
	struct sockaddr_in sc;
	int lenc;
	while (1) {
		lenc = sizeof(sc);
		printf("Czekam na klienta...");
		si = accept(s, (struct sockaddr FAR*) & sc, &lenc);
		printf("\nPolaczono z klientem...\n");

		char buf[80];
		while (recv(si, buf, 80, 0) > 0) {
			if (strcmp(buf, "KONIEC") == 0) {
				printf("Rozmowca zakonczyl czat. Wpisz KONIEC, aby zakonczyc.\n");
				closesocket(si);
				WSACleanup();
				return 0;
			}
			int dlug = strlen(buf);
			if (dlug > 0) {
				if (strcmp(buf, "a") == 0)printf("lewo\n");
				if (strcmp(buf, "d") == 0)printf("prawo\n");
				if (strcmp(buf, "w") == 0)printf("gora\n");
				if (strcmp(buf, "s") == 0)printf("dol\n");
				//printf("Nowa wiadomosc: %s\n", buf);
			}
		}
		printf("Utracono polaczenie z klientem...\n");
	}
	return 0;
}
HANDLE watek(int port) {
	DWORD threadID;
	HANDLE threadHandle = CreateThread(NULL, 0,
		funkcja_watku, SpawnParams(port), 0, &threadID);
	return threadHandle;
}
int wmain() {
	//char adresIP[20];
	//printf("Podaj adres IP:");
	//scanf_s("%20s", adresIP, 20);
	int portserwer = 2000;
	printf("Podaj numer portu z ktorym sie polaczysz:");
	scanf_s("%d", &portserwer);
	int port = 2001;
	printf("Podaj adres IP:");
	printf("Podaj numer portu Twojego serwera:");
	scanf_s("%d", &port);
	HANDLE handle;
	handle = watek(port);
	SOCKET s;
	struct sockaddr_in sa;
	WSADATA wsas;
	WORD wersja;
	wersja = MAKEWORD(2, 0);
	WSAStartup(wersja, &wsas);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void*)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(portserwer);
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");
	while (connect(s, (struct sockaddr FAR*) & sa, sizeof(sa)) == SOCKET_ERROR) {
		printf("...");
	}
	int dlug;
	char buf[80];
	char tab[size][size];
	while (1) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				tab[i][j] = ' ';
				if (i == j)tab[i][j] = '*';
			}
		}
		if (fgets(buf, 80, stdin) == NULL) {
			printf("Blad wczytywania");
			continue;
		}
		dlug = strlen(buf);
		buf[dlug - 1] = '\0';
		if (send(s, buf, dlug, 0) == SOCKET_ERROR) {
			printf("Serwer zostal zamkniety\n");
			break;
		}
		if (strcmp(buf, "KONIEC") == 0)break;
		system("cls");
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				printf("%c", tab[i][j]);
			}
			printf("\n");
		}
	}
	closesocket(s);
	WSACleanup();
	WaitForSingleObject(handle, INFINITE);
	return 0;
}