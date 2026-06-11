#pragma once
#include "order.h"
#include "trade.h"
#include <bits/stdc++.h>

using namespace std;

class OrderBook{
private:
    int _next_timestamp = 0;
    map<double, list<order>, greater<double>> buy_orders; // price -> list of buy orders (descending order of price)
    map<double, list<order>> sell_orders; // price -> list of sell orders (ascending order of price)
    unordered_map<int, pair<string, double>> stock_prices; // order_id -> (order_type, price)

public:
    vector<trade> add_order(const order& o);
    void cancel_order(const order& o);
    vector<trade> modify_order(const order& o);
    double get_best_bid() const;
    double get_best_ask() const;
    int get_depth(const string& order_type) const;
    pair<double, int> get_level_info(const string& order_type, int level) const;
    int get_Quantity_at_price(const string& order_type, double price) const;
};