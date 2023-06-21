#include <iostream>

#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define threadpool_size 4

sem_t x, y;
pthread_t tid;
pthread_t readerthreads[100];
pthread_t writerthreads[100];
int readercount = 0;

void *reader(void* param) {
    sem_wait(&x);
    readercount++;

    if(readercount == 1) {
        sem_wait(&y);
    }

    sem_post(&x);

    std::cout << readercount << " is inside\n";

    sleep(5);

    sem_wait(&x);
    readercount--;

    if (readercount == 0) {
        sem_post(&y);
    }

    sem_post(&x);

    std::cout << "reader leaved\n"; 

}

void *writer(void* param) {

    std::cout << "Writer is trying to enter\n";

    sem_wait(&y);

    std::cout << "Writer entered\n";

    sem_post(&y);
    
    std::cout << "Writer is leaving\n";

    pthread_exit(NULL);
}


int main(){

    int server_socket, new_socket;
    sockaddr_in server_address, client_address;
    sockaddr_storage serverStorage;

    socklen_t address_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5454);

    bind(server_socket, (sockaddr*)&server_address, sizeof(server_address));

    listen(server_socket, 50);

    pthread_t thread_id[60];
    int i = 0;

    while(true) {
        address_size = sizeof(serverStorage);

        new_socket = accept(server_socket, (sockaddr*)&serverStorage, &address_size);

        int choice = 0;
        recv(new_socket, &choice, sizeof(choice), 0);

        if(choice == 1) {
            if(pthread_create(&readerthreads[i++], NULL, reader, &new_socket) != 0) {
                std::cout << "failed to create thread\n";
            }
        }

        if (choice == 2) {
            if(pthread_create(&writerthreads[i++], NULL, writer, &new_socket) != 0) {
                std::cout << "failed to create thread\n";
            }
        }

        if(i >= 50) {

            i = 0;

            while(i < 50) {
                pthread_join(writerthreads[i++], NULL);
                pthread_join(readerthreads[i++], NULL);
            }

            i = 0;
        }
    }

    return 0;
}
