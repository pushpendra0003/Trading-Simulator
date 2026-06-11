#include <bits/stdc++.h>
#include "exchange.h"
#include "trader.h"
#include "order.h"
#include "logger.h"
#include <thread>

using namespace std;

int main(){
    OrderQueue order_queue;
    Exchange exchange(order_queue);
    thread exchange_thread(&Exchange::run, &exchange);
    
    // Give exchange a moment to start socket server
    this_thread::sleep_for(chrono::milliseconds(500));

    // Create traders with numeric IDs
    Trader trader1("T1", 1, order_queue);
    Trader trader2("T2", 2, order_queue);

    thread trader_thread1(&Trader::run, &trader1);
    thread trader_thread2(&Trader::run, &trader2);

    // Wait for traders to complete and disconnect
    trader_thread1.join();
    trader_thread2.join();
    
    logger::log("All traders completed. Waiting for remaining exchanges...");
    
    // Give exchange time to process remaining orders and send final executions
    this_thread::sleep_for(chrono::milliseconds(2000));
    
    // Shutdown exchange by sending a sentinel order
    order stop_order(-1, 0, 0.0, "stop", -1);
    order_queue.push(stop_order);
    
    exchange_thread.join();
    logger::log("Exchange stopped. Program terminating.");

    return 0;
}
