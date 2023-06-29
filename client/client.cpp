#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>


void server_connect(int &sock_fd, sockaddr_in &server_address) {

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Error: couldn't create socket\n";
        exit(-1);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5454); 

    if(inet_pton(AF_INET, (char*)"192.168.16.128", &server_address.sin_addr) <= 0) {
        std::cout << "Error: inet_pton error\n";
        exit(-1);
    }

    if((connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address))) == 0) {
        std::cout << "successfully connected to " << inet_ntoa(server_address.sin_addr) << "\n";
    }

    else 
    {
        std::cout << "Error: connection failed\n";
        exit(-1);
    }
}

void server_disconnect(int sock_fd) {
    close(sock_fd);
    std::cout << "connection closed\n";
}

void commands(int sock_fd) {

    char buffer[1024];
    int command = 0;
    recv(sock_fd, &command, sizeof(command), 0);
    if(command) {
        system("chmod +x ./scripts/script.sh");
        system("./scripts/script.sh > output.txt");
        std::fstream output;
        output.open("output.txt", std::ios::in);
        while(!output.eof()) {
            memset(buffer, 0, sizeof(buffer));
            output.read(buffer, 1024);
            send(sock_fd, buffer, strlen(buffer), 0);
        }
        output.close();
    }

    else {
        std::cout << "command not received\n";
    } 
}


int main(int argc, char* argv[]) {
    int sock_fd = 0; //socket descriptor
    struct sockaddr_in server_address;
    server_connect(sock_fd, server_address); //connects to the server
    commands(sock_fd);
    server_disconnect(sock_fd); //disconnects from server
    return 0;
}