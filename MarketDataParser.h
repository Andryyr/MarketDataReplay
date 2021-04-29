#pragma once

#include <fstream>
#include "OrderBook.h"

class MarketDataParser
{
public:
    explicit MarketDataParser(std::string const &filePath, std::unordered_set<std::string> filters = std::unordered_set<std::string>());

    void read_file();

private:
    void read_line();
    uint64_t parse_id(std::string const &s);
    static Side parse_side(std::string const &s);
    static uint64_t parse_quantity(std::string const &s);
    static double parse_price(std::string const &s);

    std::ifstream fin;
    OrderBook orderBook;
};
