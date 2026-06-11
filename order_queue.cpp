#include "order_queue.h"
#include <bits/stdc++.h>

void OrderQueue::push(const order& o){
    unique_lock<mutex> lock(mtx);
    orders_for_matching.push(o);
    cv.notify_one();
}

order OrderQueue::pop(){
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this]{ return !orders_for_matching.empty(); });
    order o = orders_for_matching.front();
    orders_for_matching.pop();
    return o;
}