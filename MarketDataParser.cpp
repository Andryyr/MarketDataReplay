#include <vector>
#include "MarketDataParser.h"

MarketDataParser::MarketDataParser(std::string const &filePath, std::unordered_set<std::string> filters)
{
    fin.open(filePath);
    orderBook = OrderBook(filters);
}

void MarketDataParser::read_file()
{
    size_t line = 1;
    while (!fin.eof())
    {
        try
        {
            read_line();
        }
        catch (std::invalid_argument &e)
        {
            throw std::invalid_argument("Wrong input in line " + std::to_string(line) + ":\n" + e.what());
        }
        line++;
    }
}

void MarketDataParser::read_line()
{
    std::string command;
    getline(fin, command);
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string delimiter = ",";
    while ((pos = command.find(delimiter)) != std::string::npos)
    {
        tokens.push_back(command.substr(0, pos));
        command.erase(0, pos + delimiter.length());
    }
    tokens.push_back(command);
    if (tokens[0] == "ORDER ADD")
    {
        if (tokens.size() == 6)
        {
            orderBook.add(parse_id(tokens[1]), tokens[2], parse_side(tokens[3]), parse_quantity(tokens[4]),
                          parse_price(tokens[5]));
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command ORDER ADD");
        }
    }
    else if (tokens[0] == "ORDER MODIFY")
    {
        if (tokens.size() == 4)
        {
            orderBook.modify(parse_id(tokens[1]), parse_quantity(tokens[2]), parse_price(tokens[3]));
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command ORDER MODIFY");
        }
    }
    else if (tokens[0] == "ORDER CANCEL")
    {
        if (tokens.size() == 2)
        {
            orderBook.cancel(parse_id(tokens[1]));
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command ORDER CANCEL");
        }
    }
    else if (tokens[0] == "SUBSCRIBE BBO")
    {
        if (tokens.size() == 2)
        {
            orderBook.subscribe_BBO(tokens[1]);
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command SUBSCRIBE BBO");
        }
    }
    else if (tokens[0] == "UNSUBSCRIBE BBO")
    {
        if (tokens.size() == 2)
        {
            orderBook.unsubscribe_BBO(tokens[1]);
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command UNSUBSCRIBE BBO");
        }
    }
    else if (tokens[0] == "SUBSCRIBE VWAP")
    {
        if (tokens.size() == 3)
        {
            orderBook.subscribe_VWAP(tokens[1], parse_quantity(tokens[2]));
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command SUBSCRIBE VWAP");
        }
    }
    else if (tokens[0] == "UNSUBSCRIBE VWAP")
    {
        if (tokens.size() == 3)
        {
            orderBook.unsubscribe_VWAP(tokens[1], parse_quantity(tokens[2]));
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command UNSUBSCRIBE VWAP");
        }
    }
    else if (tokens[0] == "PRINT")
    {
        if (tokens.size() == 2)
        {
            orderBook.print(tokens[1]);
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command PRINT");
        }
    }
    else if (tokens[0] == "PRINT_FULL")
    {
        if (tokens.size() == 2)
        {
            orderBook.print_full(tokens[1]);
        }
        else
        {
            throw std::invalid_argument("Wrong number of arguments for command PRINT_FULL");
        }
    }
    else
    {
        throw std::invalid_argument("Wrong command: " + tokens[0]);
    }
}

uint64_t MarketDataParser::parse_id(std::string const &s)
{
    try
    {
        return std::stoull(s);
    }
    catch (std::exception &e)
    {
        throw std::invalid_argument("Expected unsigned number, received " + s);
    }
}

Side MarketDataParser::parse_side(std::string const &s)
{
    if (s == "Buy")
    {
        return Side::BUY;
    }
    if (s == "Sell")
    {
        return Side::SELL;
    }
    throw std::invalid_argument("Expected Buy/Sell, received " + s);
}

uint64_t MarketDataParser::parse_quantity(std::string const &s)
{
    try
    {
        return std::stoull(s);
    }
    catch (std::exception &e)
    {
        throw std::invalid_argument("Expected unsigned number, received " + s);
    }
}

double MarketDataParser::parse_price(std::string const &s)
{
    double res;
    try
    {
        res = std::stod(s);
    }
    catch (std::exception &e)
    {
        throw std::invalid_argument("Expected float number, received " + s);
    }
    if (res < 0)
    {
        throw std::invalid_argument("Price cannot be negative");
    }
    return res;
}
