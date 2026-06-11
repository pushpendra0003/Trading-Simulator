// This file implements a simple logger class that provides thread-safe logging functionality. The logger uses a mutex to ensure that log messages from different threads do not interleave and are printed in a coherent manner. The log function takes a string message as input and prints it to the standard output.
#include "logger.h"
#include <bits/stdc++.h>

using namespace std;

mutex logger::log_mutex;

void logger::log(const string& message) {
    lock_guard<mutex> lock(log_mutex);
    cout << message << endl;
}

