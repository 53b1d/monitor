#include <iostream>

#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>

sem_t x, y;
int readercount = 0;


typedef struct writer_parameters {
    int connection_id;
} writer_parameters;

void *reader(void* parameters) {
    sem_wait(&x);
    readercount++;

    if(readercount == 1) {
        sem_wait(&y);
    }

    sem_post(&x);

    std::cout << readercount << " is inside\n";

    
    //read();

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

    writer_parameters w1;
    w1.connection_id = ((writer_parameters*)parameters)->connection_id;
    std::cout << "Writer is trying to enter\n";

    sem_wait(&y);

    std::cout << "Writer entered\n";

    char message[] = "hello from server";
    send(w1.connection_id, message, sizeof(message), 0);
    sem_post(&y);
    
    std::cout << "Writer is leaving\n";

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

    getsockname(server_fd, (struct sockaddr*)&server_address, &server_address_length);

    listen(server_fd, 50);

    std::cout << "Listening on " << inet_ntoa(server_address.sin_addr) << " " << htons(server_address.sin_port) << "\n";

    int connection_id = 0;

    while(true) {

        connection_fd = accept(server_fd, (sockaddr*)&server_storage, &server_size);

        char choice[1024];
        recv(connection_fd, &choice, sizeof(choice), 0);
        std::cout << choice << "\n";
        writer_parameters w1;
        w1.connection_id = connection_fd;
        pthread_create(&read_threads[connection_id++], NULL, writer, (void*)&w1);

        // if(choice == 1) {
        //     std::cout << "Read request from " << connection_fd << "\n";
        //     pthread_create(&read_threads[connection_id++], NULL, reader, &connection_fd);
        // }

        // if(choice == 2) {
        //     std::cout << "Write request from " << connection_fd << "\n";
        //     pthread_create(&read_threads[connection_id++], NULL, writer, &connection_fd);
        // }

        if(connection_id == 3) {
            connection_id = 0;
            while(connection_id < 4) {
                pthread_join(write_threads[connection_id++], NULL);
                pthread_join(read_threads[connection_id++], NULL);
            }
            connection_id = 0;
        }
    }

    return 0;
}
