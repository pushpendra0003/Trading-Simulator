#pragma once
#include <bits/stdc++.h>

using namespace std;

class logger{
public:
    static void log(const string& message);

private:
    static mutex log_mutex;
};