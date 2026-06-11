#pragma once
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <string>

class SocketServer {
private:
    int server_socket;
    int listen_port;
    std::vector<int> client_sockets;
    std::mutex clients_mutex;
    std::map<int, int> trader_sockets;  // trader_id -> socket_fd
    
    void handle_client_registration(int client_socket);
    
public:
    SocketServer(int port = 9000);
    ~SocketServer();
    
    bool start_server();
    void accept_connections();
    void send_execution(int trader_id, int order_id, double price, int quantity);
    void close_connection(int trader_id);
};
