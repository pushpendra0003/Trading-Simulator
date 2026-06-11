#pragma once
#include <bits/stdc++.h>

using namespace std;

struct trade{
    int buy_id;
    int sell_id;
    int quantity;
    double price;
    int buy_trader_id;
    int sell_trader_id;
    
    trade(int bid, int sid, int qty, double pr, int buy_tid = -1, int sell_tid = -1) 
        : buy_id(bid), sell_id(sid), quantity(qty), price(pr), buy_trader_id(buy_tid), sell_trader_id(sell_tid) {}
};