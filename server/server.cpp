#include <iostream>

#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>



// #define threadpool_size 4

// sem_t x, y;
// pthread_t tid;
// pthread_t readerthreads[100];
// pthread_t writerthreads[100];
// int readercount = 0;

// void *reader(void* param) {
//     sem_wait(&x);
//     readercount++;

//     if(readercount == 1) {
//         sem_wait(&y);
//     }

//     sem_post(&x);

//     std::cout << readercount << " is inside\n";

//     sleep(5);

//     sem_wait(&x);
//     readercount--;

//     if (readercount == 0) {
//         sem_post(&y);
//     }

//     sem_post(&x);

//     std::cout << "reader leaved\n"; 

//     return NULL;
// }

// void *writer(void* param) {

//     std::cout << "Writer is trying to enter\n";

//     sem_wait(&y);

//     std::cout << "Writer entered\n";

//     sem_post(&y);
    
//     std::cout << "Writer is leaving\n";

//     pthread_exit(NULL);
// }


int main(){

    int server_fd = 0, conn_fd = 0;
    sockaddr_in server_address;
    
    time_t ticks;
    char message[1024];

    //socklen_t address_size;
    //sem_init(&x, 0, 1);
    //sem_init(&y, 0, 1);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5454);

    bind(server_fd, (sockaddr*)&server_address, sizeof(server_address));

    listen(server_fd, 50);

    //pthread_t thread_id[60];

    //int i = 0;

    while(true) {

        conn_fd = accept(server_fd, (sockaddr*)NULL, NULL);

        ticks = time(NULL);

        snprintf(message, sizeof(message), "%.24s\r\n", ctime(&ticks));
        write(conn_fd, message, strlen(message));

        close(conn_fd);
        sleep(1);
    }

    return 0;
}
