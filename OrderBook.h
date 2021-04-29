#pragma once

#include <cstdint>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include "Order.h"

struct cmp_for_buy {
    bool operator() (const std::shared_ptr<Order> &a, const std::shared_ptr<Order> &b);
};

struct cmp_for_sell {
    bool operator() (const std::shared_ptr<Order> &a, const std::shared_ptr<Order> &b);
};

class OrderBook
{
public:
    OrderBook();

    explicit OrderBook(std::unordered_set<std::string> &filters);

    void add(uint64_t id, std::string const &symbol, Side side, uint64_t quantity, double price);
    void cancel(uint64_t id);
    void modify(uint64_t id, uint64_t quantity, double price);
    void subscribe_VWAP(std::string const &symbol, uint64_t quantity);
    void unsubscribe_VWAP(std::string const &symbol, uint64_t quantity);
    void subscribe_BBO(std::string const &symbol);
    void unsubscribe_BBO(std::string const &symbol);
    void print(std::string const &symbol);
    void print_full(std::string const &symbol);

private:
    void add(const std::shared_ptr<Order>& new_order_ptr);

    std::unordered_map<std::string, std::multiset<uint64_t>> VWAP_subscriptions;
    std::unordered_set<std::string> BBO_subscriptions;
    std::unordered_map<std::string, std::set<std::shared_ptr<Order>, cmp_for_buy>> orders_for_buy;
    std::unordered_map<std::string, std::set<std::shared_ptr<Order>, cmp_for_sell>> orders_for_sell;
    std::unordered_map<uint64_t, std::shared_ptr<Order>> order_by_id;
    std::unordered_set<std::string> filters;
};
