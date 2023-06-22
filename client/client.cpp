#include <iostream>
#include <cstring>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

void* client_thread(void *args) {
    
    int sock_fd = 0;
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
    }

    std::cout << "Connection established\n";

    int choice = *((int*)args);
    send(sock_fd, &choice, sizeof(choice), 0);

    close(sock_fd);
    pthread_exit(NULL);

    return 0; 
}

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("use: %s <ip of server>\n", argv[0]);
        return 1;
    }


    while(true) {

        int choice = 0;
        std::cout << "Enter choice:\n";
        std::cin >> choice;
        pthread_t thread;

        if(choice == 1) {
            pthread_create(&thread, NULL, client_thread, &choice);
        }
        
        else if(choice == 2) {
            pthread_create(&thread, NULL, client_thread, &choice);
        }

        else 
        {
            std::cout << "Bad choice\n";
        }

        pthread_join(thread, NULL);
        
    }
    
    return 0;
}