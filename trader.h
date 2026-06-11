#pragma once
#include <bits/stdc++.h>
#include "order_queue.h"
#include "socket_client.h"

using namespace std;

class Trader{
public:
    Trader(string id, int tid, OrderQueue& oq);
    void run();

private:
    string trader_id;
    int trader_numeric_id;
    OrderQueue& order_queue;
    SocketClient socket_client;
};