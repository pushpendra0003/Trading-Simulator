#pragma once
#include <bits/stdc++.h>

using namespace std;

struct order{
    int order_id;
    int quantity;
    double price;
    string order_type;
    int timestamp;
    int trader_id;  // Which trader placed this order

    order(int id, int qty, double pr, string type, int trader_tid = -1, int ts=0);
};