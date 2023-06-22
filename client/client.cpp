#include <stdio.h>
#include <cstring>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

// void *client_thread(void *args) {
//     int client_request = *((int*)args);
//     int network_socket;

//     network_socket = socket(af_inet, sock_stream, 0);

//     struct sockaddr_in server_address;
//     server_address.sin_family = af_inet;
//     server_address.sin_addr.s_addr = inaddr_any;
//     server_address.sin_port = htons(5454);

//     int connection_status = connect(network_socket, (sockaddr*)&server_address, sizeof(server_address));

//     std::cout << "connection established\n";

//     send(network_socket, &client_request, sizeof(client_request), 0);

//     close(network_socket);
    
//     pthread_exit(null);
    
//     return 0;
// }

int main(int argc, char* argv[]) {

    int sock_fd = 0;
    char message[1024];
    struct sockaddr_in server_address;

    if(argc != 2) {
        printf("use: %s <ip of server>\n", argv[0]);
        return 1;
    }

    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)) {
        printf("Error: couldn't create socket\n");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5454);

    memset(&server_address, '0', sizeof(server_address));

    if(inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        printf("Error: inet_pton error occured\n");
        return 1;
    }

    if(connect(sock_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("Error: connection failed\n");
        return 1;
    }

    int n = 0;

    while(n = read(sock_fd, message, sizeof(message) - 1) > 0) {
        message[n] = 0;
        printf("%s", message);
    }

    if(n < 0)
    {
        printf("Error: read from socket\n");
    } 

    // pthread_t tid;

    // int client_request = 2;
    // pthread_create(&tid, NULL, client_thread, &client_request);
    // sleep(20);

    // pthread_join(tid, NULL);
    
    return 0;
}