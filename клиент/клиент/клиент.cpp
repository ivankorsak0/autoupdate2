#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace std;

string readFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return content;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock" << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Failed to connect to server" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    string clientContent = readFile("client.txt");
    send(clientSocket, clientContent.c_str(), clientContent.size(), 0);

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "Received: " << buffer << endl;

        if (string(buffer) == "dif version") {
            ofstream outFile("downloaded.exe", ios::binary);
            outFile.write(buffer, bytesReceived);
            cout << "App was updated" << endl;
        }
        if (string(buffer) == "equal") {
            cout << "equal version";
        }
    }
    else {
        cerr << "Failed to receive data" << endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
