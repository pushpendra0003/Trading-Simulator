//learning how to implement an order book in C++ for a trading system. The order book will support adding, canceling, and modifying orders, as well as querying the best bid/ask and depth of the order book at different levels. 
#include "order_book.h"
#include <bits/stdc++.h>

using namespace std;
vector<trade> OrderBook::add_order(const order& o_const){
    order o = o_const;  // make mutable copy
    vector<trade> trades_executed;
    o.timestamp = _next_timestamp++;
    if(o.order_type == "buy"){
        //match with sell orders;
        auto it = sell_orders.begin();
        while(it != sell_orders.end() && it->first <= o.price && o.quantity > 0){
            auto& sell_list = it->second;
            order& sell_order = sell_list.front();
            int trade_qty = min(o.quantity, sell_order.quantity);
            trades_executed.emplace_back(o.order_id, sell_order.order_id, trade_qty, sell_order.price, o.trader_id, sell_order.trader_id);
            o.quantity -= trade_qty;
            sell_order.quantity -= trade_qty;
            if(sell_order.quantity == 0){
                stock_prices.erase(sell_order.order_id);
                sell_list.pop_front();
            }
            if(sell_list.empty()){
                it = sell_orders.erase(it);
            }
            else{
                ++it;
            }
        }
        if(o.quantity > 0){
            buy_orders[o.price].push_back(o);
            stock_prices[o.order_id] = {"buy", o.price};
        }
    }
    else{
        //match with buy orders;
        auto it = buy_orders.begin();
        while(it != buy_orders.end() && it->first >= o.price && o.quantity > 0){
            auto& buy_list = it->second;
            order& buy_order = buy_list.front();
            int trade_qty = min(o.quantity, buy_order.quantity);
            trades_executed.emplace_back(buy_order.order_id, o.order_id, trade_qty, buy_order.price, buy_order.trader_id, o.trader_id);
            o.quantity -= trade_qty;
            buy_order.quantity -= trade_qty;
            if(buy_order.quantity == 0){
                stock_prices.erase(buy_order.order_id);
                buy_list.pop_front();
            }
            if(buy_list.empty()){
                it = buy_orders.erase(it);
            }
            else{
                ++it;
            }
        }
        if(o.quantity > 0){
            sell_orders[o.price].push_back(o);
            stock_prices[o.order_id] = {"sell", o.price};
        }
    }
    return trades_executed;
}

void OrderBook::cancel_order(const order& o){
    auto sp_it = stock_prices.find(o.order_id);
    if(sp_it != stock_prices.end()){
        string type = sp_it->second.first;
        double price = sp_it->second.second;

        if(type == "buy"){
            auto& buy_list = buy_orders[price];
            for(auto list_it = buy_list.begin(); list_it != buy_list.end(); ++list_it){
                if(list_it->order_id == o.order_id){
                    buy_list.erase(list_it);
                    break;
                }
            }
        }
        else{
            auto& sell_list = sell_orders[price];
            for(auto list_it = sell_list.begin(); list_it != sell_list.end(); ++list_it){
                if(list_it->order_id == o.order_id){
                    sell_list.erase(list_it);
                    break;
                }
            }
        }

        stock_prices.erase(sp_it);
        return;
    }
}

vector<trade> OrderBook::modify_order(const order& o){
    cancel_order(o);
    return add_order(o);
}

double OrderBook::get_best_bid() const{
    if(buy_orders.empty()) return -1;
    return buy_orders.begin()->first;
}

double OrderBook::get_best_ask() const{
    if(sell_orders.empty()) return -1;
    return sell_orders.begin()->first;
}

int OrderBook::get_depth(const string& order_type) const{
    int depth = 0;
    if(order_type == "buy"){
        depth = buy_orders.size();
    }
    else if(order_type == "sell"){
        depth = sell_orders.size();
    }
    return depth;
}

pair<double, int> OrderBook::get_level_info(const string& order_type, int level) const{
    if(order_type == "buy"){
        auto it = buy_orders.begin();
        advance(it, level-1);
        if(it != buy_orders.end()){
            int total_qty = 0;
            for(const auto& o : it->second){
                total_qty += o.quantity;
            }
            return make_pair(it->first, total_qty);
        }
    }
    else if(order_type == "sell"){
        auto it = sell_orders.begin();
        advance(it, level-1);
        if(it != sell_orders.end()){
            int total_qty = 0;
            for(const auto& o : it->second){
                total_qty += o.quantity;
            }
            return make_pair(it->first, total_qty);
        }
    }
    return make_pair(-1, -1); // invalid level
}

int OrderBook::get_Quantity_at_price(const string& order_type, double price) const{
    int total_qty = 0;
    if(order_type == "buy"){
        auto it = buy_orders.find(price);
        if(it != buy_orders.end()){
            for(const auto& o : it->second){
                total_qty += o.quantity;
            }
        }
    }
    else if(order_type == "sell"){
        auto it = sell_orders.find(price);
        if(it != sell_orders.end()){
            for(const auto& o : it->second){
                total_qty += o.quantity;
            }
        }
    }
    return total_qty;
}
