#pragma once

#include <cstdint>
#include <string>

enum class Side {BUY, SELL};

class Order
{
public:
    Order(uint64_t id, std::string const &symbol, Side side, uint64_t quantity, double price);

    void modify(uint64_t quantity, double price);

    uint64_t id;
    std::string symbol;
    Side side;
    uint64_t quantity;
    double price;
};
