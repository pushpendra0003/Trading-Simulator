#include "order.h"

using namespace std;

order::order(int id, int qty, double pr, string type, int trader_tid, int ts)
    : order_id(id), quantity(qty), price(pr), order_type(type), timestamp(ts), trader_id(trader_tid) {}
