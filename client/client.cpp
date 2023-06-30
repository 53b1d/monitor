#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

void setX(const std::string path) {
    std::string command = "chmod +x ";
    command += path;
    system(command.c_str());
}

void execute(std::string path) {
    std::string command = path;
    command += " > output.txt";
    system(command.c_str());
}

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
    while(true) {
        int n = recv(sock_fd, buffer, sizeof(buffer), 0);
        
        if (n <= 0) {
            // Connection closed by the server
            break;
        }

        buffer[n - 1] = '.';
        buffer[n] = 's';
        buffer[n + 1] = 'h';
        buffer[n + 2] = 0;
        
        std::string path = "./scripts/";
        std::string command(buffer);
        path += command;
        
        setX(path);
        execute(path);
        
        std::fstream output;
        output.open("output.txt", std::ios::in);
        while(!output.eof()) {
            memset(buffer, 0, sizeof(buffer));
            output.read(buffer, 1024);
            send(sock_fd, buffer, strlen(buffer), 0);
        }
        output.close();
        system ("rm output.txt");
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