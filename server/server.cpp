#include "server.h"

void process_data(int client_fd) {
    std::cout << "Processing request for client" << client_fd << "\n";
}

void server::handle_client(int client_fd) {
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_fd); //add client to list
    }

    pollfd poll_fd {client_fd, POLLIN | POLLRDHUP, 0}; //set pollfd for client

    while(running) {
        //loop until client disconnects 
        int num_ready = poll(&poll_fd, 1, -1);
        if(num_ready < 0) {
            perror("poll");
            break;
        }
        else if (num_ready == 0) {
            continue;
        }

        if(poll_fd.revents & POLLRDHUP) {
            std::cout << "client " << client_fd << " disconnected\n";
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
           thread_pool->enqueue(process_data, client_fd); 
        }
    }
}

void server::accept_connections() {
    while(running) { //loop and accept incoming connections
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_fd = accept(server_fd, (sockaddr*)&client_address, &client_address_size);
        if (client_fd < 0) {
            perror("accept failed\n");
            continue;
        }
        
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            if(clients.size() >= MAX_CLIENTS) { //check if max number of clients has been reached
                std::cout << "max number of clients reached\n";
                close(client_fd);
                continue;
            }
        }

        if(!running) {
            close(client_fd);
            break;
        }    

        thread_pool->enqueue(std::bind(&server::handle_client, this, client_fd)); //handle the new connection
    }
}

void server::handle_cli() {
    std::string input;
    while (running) {
        input = readline("> ");
        add_history(input.c_str());

        std::istringstream iss(input);
        std::vector<std::string> tokens(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());

        if (tokens.empty()) {
            continue;
        }

        std::string command = tokens[0];

        if (command == "clients") {
            // Handle clients command
            // ...
        } else if (command == "exit") {
            // Handle exit command
            // ...
        } else if (command == "services") {
            // Handle services command
            // ...
        } else if (command == "inject") {
            // Handle inject command
            if (tokens.size() < 3) {
                std::cout << "Invalid command format. Usage: inject <client> <script-file>" << std::endl;
                continue;
            }

            int clientID = std::stoi(tokens[1]);
            std::string scriptFile = tokens[2];

            {
                std::lock_guard<std::mutex> lock(clients_mutex);

                if (clientID < 0 || clientID >= clients.size()) {
                    std::cout << "Invalid client ID" << std::endl;
                    continue;
                }

                std::ifstream file("stats.sh");
                if (!file) {
                    std::cout << "Script file not found" << std::endl;
                    continue;
                }

                std::string scriptContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

                // Send the script to the client
                int client = clients[clientID];
                send(client, scriptContent.c_str(), scriptContent.size(), 0);
                std::cout << "Script injected to client " << clientID << std::endl;
            }
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }
}


server::server()
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd < 0) {
        perror("socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(5454);

    this->thread_pool = new threadpool(3);
}

void server::init()
{
    running = true;

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

    std::thread cli_thread(&server::handle_cli, this);
    cli_thread.detach();
}

server::~server() {
    delete this->thread_pool;
    if (!running) {
        close(server_fd);
    }
}
