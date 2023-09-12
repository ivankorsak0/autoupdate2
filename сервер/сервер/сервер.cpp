#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

bool compareFiles(const string& file1, const string& file2) {
    ifstream stream1(file1, ios::binary);
    ifstream stream2(file2, ios::binary);

    if (!stream1.is_open() || !stream2.is_open()) {
        return false;
    }

    return istreambuf_iterator<char>(stream1) == istreambuf_iterator<char>() &&
        istreambuf_iterator<char>(stream2) == istreambuf_iterator<char>();
}
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

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Failed to bind socket" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Failed to listen on socket" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressSize);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed to accept client connection" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }



    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';

        ofstream outFile("received.txt");
        outFile.write(buffer, bytesReceived);
        outFile.close();
        string serverContent = readFile("server.txt");
        string clientContent = readFile("received.txt");


        if (serverContent == clientContent){
            const char* message = "equal";
            send(clientSocket, message, strlen(message), 0);
        }
        else {
            const char* message = "dif version";
            send(clientSocket, message, strlen(message), 0);
            ifstream exeFile("server.exe", ios::binary | ios::ate);
            if (exeFile.is_open()) {
                streamsize size = exeFile.tellg();
                exeFile.seekg(0, ios::beg);

                char* buffer = new char[size];
                exeFile.read(buffer, size);
                exeFile.close();

                send(clientSocket, buffer, size, 0);

                delete[] buffer;
            }
        }
        remove("received.txt");
    }
    else {
        cerr << "Failed to receive data" << endl;
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
