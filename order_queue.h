#pragma once
#include <bits/stdc++.h>
#include "order.h"

using namespace std;


class OrderQueue{
private:
    queue<order> orders_for_matching;
    mutex mtx;
    condition_variable cv;
public:
    void push(const order& o);
    order pop();
};
