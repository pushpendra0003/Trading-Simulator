#include "logger.h"
#include <bits/stdc++.h>

using namespace std;

mutex logger::log_mutex;

void logger::log(const string& message) {
    lock_guard<mutex> lock(log_mutex);
    cout << message << endl;
}

