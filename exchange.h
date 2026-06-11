#pragma once
#include <bits/stdc++.h>
#include "order_queue.h"
#include "order_book.h"
#include "socket_server.h"

using namespace std;

class Exchange{
private:
    OrderQueue& order_queue;
    OrderBook order_book;
    SocketServer socket_server;
    int next_order_id = 1;

public:
    Exchange(OrderQueue& oq);
    void run();
    void process(const order& ob);
};
