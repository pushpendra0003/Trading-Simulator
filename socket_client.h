#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>

struct ExecutionMessage {
    int order_id;
    double price;
    int quantity;
};

class SocketClient {
private:
    int socket_fd;
    std::string server_host;
    int server_port;
    int trader_id;
    std::vector<ExecutionMessage> executions;
    std::mutex exec_mutex;
    bool connected;
    
public:
    SocketClient(const std::string& host = "127.0.0.1", int port = 9000);
    ~SocketClient();
    
    bool connect_to_server(int tid);
    void start_listening();
    std::vector<ExecutionMessage> get_executions();
    void clear_executions();
    bool is_connected() const;
};
