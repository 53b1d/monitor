#include <iostream>

#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"

sem_t x, y;
int readercount = 0;

typedef struct thread_parameters {
    int connection_id;
    sockaddr_storage *sock_address;
    socklen_t addr_size;
} thread_parameters;

void *reader(void* parameters) {

    thread_parameters w1;
    w1.connection_id = ((thread_parameters*)parameters)->connection_id;

    sem_wait(&x);

    readercount++;

    if(readercount == 1) {
        sem_wait(&y);
    }

    sem_post(&x);

    std::cout << readercount << " is inside\n";
    char buffer[1024];
    std::cout << "client "<< w1.connection_id << ": ";
    int bytes_read = 0;
    while(bytes_read = read(w1.connection_id, buffer, sizeof(buffer))){
        std::cout << buffer << "\n";
    }
     
    sem_wait(&x);
    readercount--;

    if (readercount == 0) {
        sem_post(&y);
    }

    sem_post(&x);

    std::cout << "reader leaved\n"; 

    return NULL;
}

void *writer(void *parameters) {

    thread_parameters tp;
    tp.connection_id = ((thread_parameters*)parameters)->connection_id;
    tp.sock_address = ((thread_parameters*)parameters)->sock_address;
    tp.addr_size = ((thread_parameters*)parameters)->addr_size;

    getpeername(tp.connection_id, (sockaddr*)&tp.sock_address, &tp.addr_size);
    char ipstr[INET_ADDRSTRLEN];

    struct sockaddr_in *s = (struct sockaddr_in*)&tp.sock_address;
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

    std::cout << ipstr << " connected\n";

    std::cout << "waiting to send script to "<< tp.connection_id << "\n";

    sem_wait(&y);

    std::cout << "sending script to " << tp.connection_id << "\n";

    FILE *script = fopen("stats.sh", "r");
    int n = 0;
    char buffer[1024];
    fread(&buffer, 1024, 1, script);
    send(tp.connection_id, buffer, sizeof(buffer), 0);

    fclose(script);
    memset(buffer, 0, sizeof(buffer));
    recv(tp.connection_id, buffer, sizeof(buffer), 0);

    std::cout << buffer;

    sem_post(&y);
    
    std::cout << "script sent\n";

    pthread_exit(NULL);
}


int main(){

    int server_fd = 0;
    int connection_fd = 0;
    struct sockaddr_in server_address;
    struct sockaddr_storage server_storage;
    socklen_t server_size = sizeof(server_storage);
    

    pthread_t read_threads[40];
    pthread_t write_threads[40];

    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5454);

    bind(server_fd, (sockaddr*)&server_address, sizeof(server_address));

    socklen_t server_address_length = sizeof(server_address);

    char hostname[24];
    gethostname(hostname, 24);

    char ipaddr[17];
    struct addrinfo server_address_info, *res;
    server_address_info.ai_family = AF_INET;

    if(getaddrinfo(hostname, NULL, &server_address_info, &res)) {
        std::cout << "Failed to get ip address from host\n";
    }

    inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ipaddr, INET_ADDRSTRLEN);

    listen(server_fd, 50);

    std::cout << "server: running on " << hostname << " " << ipaddr << " " << htons(server_address.sin_port) << "\n";

    int connection_id = 0;

    while(true) {
        // connection_fd = accept(server_fd, (sockaddr*)&server_storage, &server_size);
        // thread_parameters tp;
        // tp.connection_id = connection_fd;
        // tp.sock_address = &server_storage;
        // tp.addr_size = server_size;
        // pthread_create(&read_threads[connection_id++], NULL, writer, (void*)&tp);
        
        char *line = readline("> ");
        if(!line) {
            std::cout << "error typo\n";
            break;
        }

        if(*line) {
            add_history(line);
        }

        command new_command(line);
        new_command.execute();

        free(line);
    }

    return 0;
}
