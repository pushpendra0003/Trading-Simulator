#include "socket_server.h"
#include "logger.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

SocketServer::SocketServer(int port) : server_socket(-1), listen_port(port) {}

SocketServer::~SocketServer() {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int sock : client_sockets) {
        close(sock);
    }
    if (server_socket != -1) {
        close(server_socket);
    }
}

bool SocketServer::start_server() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        logger::log("Error: Could not create socket");
        return false;
    }
    
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        logger::log("Error: setsockopt failed");
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(listen_port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        logger::log("Error: Could not bind socket to port " + std::to_string(listen_port));
        return false;
    }
    
    if (listen(server_socket, 5) < 0) {
        logger::log("Error: Listen failed");
        return false;
    }
    
    logger::log("Exchange server listening on port " + std::to_string(listen_port));
    
    std::thread accept_thread(&SocketServer::accept_connections, this);
    accept_thread.detach();
    
    return true;
}

void SocketServer::accept_connections() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            logger::log("Error: Accept failed");
            continue;
        }
        
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            client_sockets.push_back(client_socket);
        }
        
        logger::log("New connection from " + std::string(inet_ntoa(client_addr.sin_addr)));
        
        // Receive trader_id from client
        std::thread handle_thread(&SocketServer::handle_client_registration, this, client_socket);
        handle_thread.detach();
    }
}

void SocketServer::handle_client_registration(int client_socket) {
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    
    int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        try {
            int trader_id = std::stoi(std::string(buffer));
            
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                trader_sockets[trader_id] = client_socket;
            }
            
            logger::log("Trader " + std::to_string(trader_id) + " registered on socket");
        } catch (...) {
            logger::log("Error: Failed to parse trader_id");
            close(client_socket);
        }
    }
}

void SocketServer::send_execution(int trader_id, int order_id, double price, int quantity) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    if (trader_sockets.find(trader_id) == trader_sockets.end()) {
        return;  // Trader not connected
    }
    
    int sock = trader_sockets[trader_id];
    
    // Message format: order_id|price|quantity
    std::string message = std::to_string(order_id) + "|" + 
                         std::to_string(price) + "|" + 
                         std::to_string(quantity) + "\n";
    
    if (send(sock, message.c_str(), message.length(), 0) < 0) {
        logger::log("Error: Failed to send execution to trader " + std::to_string(trader_id));
        close(sock);
        trader_sockets.erase(trader_id);
    }
}

void SocketServer::close_connection(int trader_id) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    if (trader_sockets.find(trader_id) != trader_sockets.end()) {
        close(trader_sockets[trader_id]);
        trader_sockets.erase(trader_id);
    }
}
