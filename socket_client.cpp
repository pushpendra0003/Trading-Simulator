#include "socket_client.h"
#include "logger.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

SocketClient::SocketClient(const std::string& host, int port) 
    : socket_fd(-1), server_host(host), server_port(port), trader_id(-1), connected(false) {}

SocketClient::~SocketClient() {
    if (socket_fd != -1) {
        close(socket_fd);
    }
}

bool SocketClient::connect_to_server(int tid) {
    trader_id = tid;
    
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        logger::log("Trader " + std::to_string(trader_id) + ": Could not create socket");
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    if (inet_pton(AF_INET, server_host.c_str(), &server_addr.sin_addr) <= 0) {
        logger::log("Trader " + std::to_string(trader_id) + ": Invalid address");
        return false;
    }
    
    if (::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        logger::log("Trader " + std::to_string(trader_id) + ": Connection failed");
        return false;
    }
    
    // Send trader_id to server for registration
    std::string trader_id_msg = std::to_string(trader_id);
    if (send(socket_fd, trader_id_msg.c_str(), trader_id_msg.length(), 0) < 0) {
        logger::log("Trader " + std::to_string(trader_id) + ": Failed to send trader_id");
        return false;
    }
    
    connected = true;
    logger::log("Trader " + std::to_string(trader_id) + " connected to exchange");
    
    std::thread listen_thread(&SocketClient::start_listening, this);
    listen_thread.detach();
    
    return true;
}

void SocketClient::start_listening() {
    char buffer[256];
    
    while (connected) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes <= 0) {
            if (bytes < 0) {
                logger::log("Trader " + std::to_string(trader_id) + ": Socket error");
            }
            connected = false;
            break;
        }
        
        buffer[bytes] = '\0';
        
        // Parse message: order_id|price|quantity
        std::string msg(buffer);
        size_t pos1 = msg.find('|');
        size_t pos2 = msg.rfind('|');
        
        if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 < pos2) {
            try {
                int order_id = std::stoi(msg.substr(0, pos1));
                double price = std::stod(msg.substr(pos1 + 1, pos2 - pos1 - 1));
                int quantity = std::stoi(msg.substr(pos2 + 1));
                
                {
                    std::lock_guard<std::mutex> lock(exec_mutex);
                    executions.push_back({order_id, price, quantity});
                }
                
                logger::log("Trader " + std::to_string(trader_id) + " received execution: Order " + 
                           std::to_string(order_id) + " @ " + std::to_string(price) + 
                           " x" + std::to_string(quantity));
            } catch (...) {
                logger::log("Trader " + std::to_string(trader_id) + ": Parse error");
            }
        }
    }
}

std::vector<ExecutionMessage> SocketClient::get_executions() {
    std::lock_guard<std::mutex> lock(exec_mutex);
    return executions;
}

void SocketClient::clear_executions() {
    std::lock_guard<std::mutex> lock(exec_mutex);
    executions.clear();
}

bool SocketClient::is_connected() const {
    return connected;
}
