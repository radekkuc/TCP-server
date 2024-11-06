#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

int main(){
    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1){
        cerr << "Cant create a socket" << endl;
        return -1;
    }
    
    // Bind address, port to that socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1){
        cerr << "Cant bind to IP/port" << endl;
        return -2;
    }
    
    // Mark the socket for listening in
    if(listen(listening, SOMAXCONN) == -1){
        cerr << "Cant listen" << endl;
        return -3;
    }

    // Accept a call from client
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST]; //address
    char svc[NI_MAXSERV]; //port
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if(clientSocket == -1){
        cerr << "Cant accept" << endl;;
        return -4;
    }

    close(listening);

    memset(&host, 0, NI_MAXHOST);
    memset(&svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result == 0) {  
        cout << host << " connected on " << svc << endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on " << ntohs(client.sin_port) << endl;
    }


    // (OPTIONAL) Close listening socket if server server should receive one call at most
    // While receiving display message
    char buf[4096];
    while(true){
        memset(buf, 0, 4096);
        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if(bytesReceived == -1){
            cerr << "Error in recv()" << endl;
            break;
        }

        if(bytesReceived == 0){
            cerr << "Client disconnected" << endl;
            break;
        }

        cout << string(buf, 0, bytesReceived) << endl;

        // Echo message back to client 
        if (send(clientSocket, buf, bytesReceived, 0) == -1) {
            cerr << "Error in send()" << endl;
            break;
        }   

    }
    // Close socket
    close(clientSocket);

    return 0;
}