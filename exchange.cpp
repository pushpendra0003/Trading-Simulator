#include "exchange.h"
#include "trade.h"
#include "logger.h"
#include <bits/stdc++.h>

using namespace std;

Exchange::Exchange(OrderQueue& oq): order_queue(oq), socket_server(9000) {
    // Start socket server to communicate with traders
    socket_server.start_server();
    logger::log("Exchange started with socket server");
}

void Exchange::run(){
    // Keep processing orders
    // This will run until a sentinel order is received
    while(true){
        try {
            order o = order_queue.pop();
            
            // Sentinel order to stop exchange
            if (o.trader_id == -1) {
                logger::log("Exchange received stop signal. Shutting down.");
                break;
            }
            
            process(o);
        } catch (...) {
            // If queue is empty or error, continue
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

void Exchange::process(const order& ob){
    vector<trade> trades = order_book.add_order(ob);
    for(const trade& t : trades){
        logger::log("Trade executed: Buy Order ID " + to_string(t.buy_id) + 
                   " (Trader " + to_string(t.buy_trader_id) + "), Sell Order ID " + 
                   to_string(t.sell_id) + " (Trader " + to_string(t.sell_trader_id) + 
                   "), Quantity " + to_string(t.quantity) + ", Price " + to_string(t.price));
        
        // Send execution notifications to both traders via socket
        socket_server.send_execution(t.buy_trader_id, t.buy_id, t.price, t.quantity);
        socket_server.send_execution(t.sell_trader_id, t.sell_id, t.price, t.quantity);
    }
}