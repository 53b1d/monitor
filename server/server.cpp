#include <iostream>

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

#include <poll.h>

#include "command.h"
#include "threadpool.h"

#define MAX_CLIENTS 4

std::vector<int> clients;
std::mutex clients_mutex;
threadpool thread_pool(3);
int server_fd;

void handle_client(int client_fd) {
    //add client to list
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_fd);
    }

    //set pollfd for client
    pollfd poll_fd {client_fd, POLLIN | POLLRDHUP, 0};

    //loop until disconnects 
    while(true) {
        int num_ready = poll(&poll_fd, 1, -1);
        if(num_ready < 0) {
            perror("poll");
            break;
        }
        else if (num_ready == 0) {
            continue;
        }

        if(poll_fd.revents & POLLRDHUP) {
            std::cout << "client " << client_fd << "disconnected\n";
            //remove client from list 
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
            }

            close(client_fd);
            break;
        } 
        else
        {
           //process incoming data
           //thread_pool.enqueue(process_data, buffer, n); 
        }
    }
}

void accept_connections() {
    //loop and accept incoming connections
    while(true) {
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_fd = accept(server_fd, (sockaddr*)&client_address, &client_address_size);
        if (client_fd < 0) {
            perror("accept failed\n");
            continue;
        }
        
        //check if max number of clients has been reached
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            if(clients.size() >= MAX_CLIENTS) {
                std::cout << "max nr of clients reached\n";
                close(client_fd);
                continue;
            }
        }

        //start thread to handle the new connection
        thread_pool.enqueue(handle_client, client_fd);
    }
}

void handle_cli() {
    std::string input;
    while(true) {
        input = readline("> ");
        add_history(input.c_str());
        
        if(input == "clients") {
            //list of connected clients
            std::cout << "Connected clients:" << std::endl;
            for (int client : clients) {
                struct sockaddr_in addr;
                socklen_t addr_len = sizeof(addr);
                if (getpeername(client, (struct sockaddr*)&addr, &addr_len) == 0) {
                    std::cout << "  " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
                }
            }
        }

        if(input == "exit") {
            //close all client sockets
            std::lock_guard<std::mutex> lock(clients_mutex);
            for(int client : clients) {
                close(client);
            }

            close(server_fd);
            exit(0);
        }

        if(input == "services") {
            std::cout << "no implementation\n";
        }
    } 
}

int main(){
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_address { 0 };

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5454);

    if(bind(server_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        exit(1);
    }

    char hostname[24];
    gethostname(hostname, 24);

    if(listen(server_fd, 50) < 0) {
        perror("listen");
        exit(1);
    }

    std::cout << "server: running on " << hostname << " " << htons(server_address.sin_port) << "\n";

    std::thread thread_cli(&handle_cli);
    std::thread thread_clients(&handle_client);

    //thread_cli.join();
    //thread_clients.join();


    return 0;
}
