#include "Order.h"

Order::Order(uint64_t id, std::string const &symbol, Side side, uint64_t quantity, double price)
        : id(id), symbol(symbol),
          side(side),
          quantity(quantity),
          price(price)
{}

void Order::modify(uint64_t quantity, double price)
{
    this->quantity = quantity;
    this->price = price;
}
