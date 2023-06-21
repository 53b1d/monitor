#include <iostream>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

void *client_thread(void *args) {
    int client_request = *((int*)args);
    int network_socket;

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5454);

    int connection_status = connect(network_socket, (sockaddr*)&server_address, sizeof(server_address));

    std::cout << "connection established\n";

    send(network_socket, &client_request, sizeof(client_request), 0);

    close(network_socket);
    pthread_exit(NULL);
    
    return 0;
}

int main() {

    std::cout << "1. Read\n"; 
    std::cout << "2. Write\n";

    int choice;
    std::cin >> choice;
    
    pthread_t tid;

    switch(choice) {
        case 1: {
            int client_request = 1;
            pthread_create(&tid, NULL, client_thread, &client_request);
            sleep(20);
            break;
        }
        case 2: {
            int client_request = 2;
            pthread_create(&tid, NULL, client_thread, &client_request);
            sleep(20);
            break;
        }

        default:
            std::cout << "Invalid input\n";
            break;
    }

    pthread_join(tid, NULL);
    
    return 0;
}