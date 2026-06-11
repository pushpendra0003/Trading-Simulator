#include "trader.h"
#include "logger.h"
#include <bits/stdc++.h>

using namespace std;

Trader::Trader(string id, int tid, OrderQueue& oq) 
        : trader_id(id), trader_numeric_id(tid), order_queue(oq) {}

void Trader::run(){
    // Connect to exchange via socket
    if (!socket_client.connect_to_server(trader_numeric_id)) {
        logger::log("Trader " + trader_id + ": Failed to connect to exchange");
        return;
    }
    
    // Place 5 orders
    int orders_placed = 0;
    for(int i = 0; i < 5; i++){
        int qty = (i + 1) * 10;
        double price = 100.0 + i * 5;
        string type = (i % 2 == 0) ? "buy" : "sell";
        order o(trader_id[0] * 100 + i, qty, price, type, trader_numeric_id);  // Pass trader_id
        order_queue.push(o);
        orders_placed++;
        logger::log(trader_id + " placed a " + type + " order: ID " + to_string(o.order_id) + 
             ", Quantity " + to_string(o.quantity) + ", Price " + to_string(o.price));
        this_thread::sleep_for(chrono::milliseconds(200));
    }
    
    // Poll for execution confirmations
    logger::log("Trader " + trader_id + " waiting for " + to_string(orders_placed) + " executions");
    
    int executions_received = 0;
    int poll_count = 0;
    while (executions_received < orders_placed && poll_count < 500) {  // timeout after ~50 seconds
        auto execs = socket_client.get_executions();
        executions_received = execs.size();
        
        if (executions_received < orders_placed) {
            this_thread::sleep_for(chrono::milliseconds(100));  // Poll every 100ms
            poll_count++;
        }
    }
    
    auto final_execs = socket_client.get_executions();
    logger::log("Trader " + trader_id + " received " + to_string(final_execs.size()) + " executions. Terminating.");
    
    for (const auto& exec : final_execs) {
        logger::log("  - Order " + to_string(exec.order_id) + " executed @ " + 
                   to_string(exec.price) + " for " + to_string(exec.quantity) + " units");
    }
}