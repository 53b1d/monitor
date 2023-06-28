#include <iostream>
#include <thread>

#include <semaphore.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h>
#include <stdlib.h>  

//command input
#include <readline/readline.h>
#include <readline/history.h>
#include <vector>

#include "command.h"

sem_t x, y;
int readercount = 0;

typedef struct serverstorage_p {
    int server_fd;
    sockaddr_storage *sock_address;
    socklen_t addr_size;
    std::vector<int> *clients;
} serverstorage_p;


void accepter(void *parameters) {
    while(true) {
        int client_fd = 0;
        serverstorage_p *sp = (serverstorage_p*)parameters;
        client_fd = accept(sp->server_fd, (sockaddr*)sp->sock_address, &sp->addr_size);
        sp->clients->push_back(client_fd);
    }
}

void cli_handler(void *parameters) {
    serverstorage_p* sp = (serverstorage_p*)parameters;
    while(true) {
        std::string line = readline("> ");
        add_history(line.c_str());

        if(line == "clients") {
                // print list of connected clients
            std::cout << "Connected clients:" << std::endl;
            for (int client : *(sp->clients)) {
                struct sockaddr_in addr;
                socklen_t addr_len = sizeof(addr);
                if (getpeername(client, (struct sockaddr*)&addr, &addr_len) == 0) {
                    std::cout << "  " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
                }
            }
        }
    } 
}

int main(){

    //sem_init(&x, 0, 1);
    //sem_init(&y, 0, 1);

    int server_fd = 0;
    struct sockaddr_in server_address;
    struct sockaddr_storage server_storage;
    socklen_t server_size = sizeof(server_storage);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5454);

    bind(server_fd, (sockaddr*)&server_address, sizeof(server_address));

    char hostname[24];
    gethostname(hostname, 24);

    listen(server_fd, 50);

    std::cout << "server: running on " << hostname << " " << htons(server_address.sin_port) << "\n";

    serverstorage_p server_parameters;
    server_parameters.server_fd = server_fd;
    server_parameters.sock_address = &server_storage;
    server_parameters.addr_size = sizeof(server_storage);

    std::vector<int> clients;
    server_parameters.clients = &clients;

    std::thread thread_clients(&accepter, &server_parameters);
    std::thread thread_cli(&cli_handler, &server_parameters);

    thread_clients.join();
    thread_cli.join();

    return 0;
}
