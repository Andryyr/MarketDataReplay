#include <utility>
#include <vector>
#include <iostream>
#include "OrderBook.h"

OrderBook::OrderBook() = default;

OrderBook::OrderBook(std::unordered_set<std::string> &filters) : filters(filters) { }

void OrderBook::add(std::shared_ptr<Order> const & new_order_ptr)
{
    order_by_id[new_order_ptr->id] = new_order_ptr;
    if (new_order_ptr->side == Side::BUY)
    {
        orders_for_buy[new_order_ptr->symbol].insert(new_order_ptr);
    }
    else
    {
        orders_for_sell[new_order_ptr->symbol].insert(new_order_ptr);
    }
}

void OrderBook::add(uint64_t id, std::string const &symbol, Side side, uint64_t quantity, double price)
{
    if (order_by_id.find(id) != order_by_id.end())
    {
        throw std::invalid_argument("This id already exists");
    }
    std::shared_ptr<Order> new_order_ptr = std::make_shared<Order>(id, symbol, side, quantity, price);
    add(new_order_ptr);
}

void OrderBook::cancel(uint64_t id)
{
    if (order_by_id.find(id) == order_by_id.end())
    {
        throw std::invalid_argument("This id does not exist");
    }
    std::shared_ptr<Order> target_order_ptr = order_by_id[id];
    if (target_order_ptr->side == Side::BUY)
    {
        orders_for_buy[target_order_ptr->symbol].erase(target_order_ptr);
    }
    else
    {
        orders_for_sell[target_order_ptr->symbol].erase(target_order_ptr);
    }
    order_by_id.erase(id);
}

void OrderBook::modify(uint64_t id, uint64_t quantity, double price)
{
    if (order_by_id.find(id) == order_by_id.end())
    {
        throw std::invalid_argument("This id does not exist");
    }
    std::shared_ptr<Order> target_order_ptr = order_by_id[id];
    cancel(id);
    target_order_ptr->modify(quantity, price);
    add(target_order_ptr);
}

void OrderBook::print(std::string const &symbol)
{
    std::cout << "Bid\t\t\tAsk\n";

    auto buy_order_it = orders_for_buy[symbol].begin();
    auto sell_order_it = orders_for_sell[symbol].begin();
    uint64_t left_to_buy = 0, left_to_sell = 0;
    std::vector<double> vwap_buy, vwap_sell;
    std::vector<bool> is_vwap_buy_complite, is_vwap_sell_complite;
    size_t vwap_buy_res_ind = 0, vwap_sell_res_ind = 0;
    std::multiset<uint64_t>::iterator vwap_buy_it, vwap_sell_it;

    if (VWAP_subscriptions.find(symbol) != VWAP_subscriptions.end())
    {
        vwap_buy_it = VWAP_subscriptions[symbol].begin();
        vwap_sell_it = vwap_buy_it;
        left_to_buy = *vwap_buy_it;
        left_to_sell = left_to_buy;
        vwap_buy.resize(VWAP_subscriptions[symbol].size(), 0);
        vwap_sell.resize(VWAP_subscriptions[symbol].size(), 0);
        is_vwap_buy_complite.resize(VWAP_subscriptions[symbol].size(), false);
        is_vwap_sell_complite.resize(VWAP_subscriptions[symbol].size(), false);
    }

    while (buy_order_it != orders_for_buy[symbol].end() || sell_order_it != orders_for_sell[symbol].end())
    {
        double last_buy_price = 0, last_sell_price = 0;
        uint64_t buy_volume = 0, sell_volume = 0;
        if (buy_order_it != orders_for_buy[symbol].end())
        {
            last_buy_price = (*buy_order_it)->price;
        }
        if (sell_order_it != orders_for_sell[symbol].end())
        {
            last_sell_price = (*sell_order_it)->price;
        }

        while (buy_order_it != orders_for_buy[symbol].end() && (*buy_order_it)->price == last_buy_price)
        {
            buy_volume += (*buy_order_it)->quantity;
            buy_order_it++;
        }
        uint64_t buy_volume_left = buy_volume;
        if (VWAP_subscriptions.find(symbol) != VWAP_subscriptions.end())
        {
            while (vwap_buy_it != VWAP_subscriptions[symbol].end() && buy_volume_left > 0)
            {
                uint64_t tmp_volume = std::min(left_to_buy, buy_volume_left);
                vwap_buy[vwap_buy_res_ind] += last_buy_price * tmp_volume;
                left_to_buy -= tmp_volume;
                buy_volume_left -= tmp_volume;
                if (left_to_buy == 0)
                {
                    is_vwap_buy_complite[vwap_buy_res_ind] = true;
                    left_to_buy = *vwap_buy_it;
                    vwap_buy_it++;
                    vwap_buy_res_ind++;
                    if (vwap_buy_it != VWAP_subscriptions[symbol].end())
                    {
                        left_to_buy = *vwap_buy_it - left_to_buy;
                        vwap_buy[vwap_buy_res_ind] = vwap_buy[vwap_buy_res_ind - 1];
                    }
                }
            }
        }

        while (sell_order_it != orders_for_sell[symbol].end() && (*sell_order_it)->price == last_sell_price)
        {
            sell_volume += (*sell_order_it)->quantity;
            sell_order_it++;
        }
        uint64_t sell_volume_left = sell_volume;
        if (VWAP_subscriptions.find(symbol) != VWAP_subscriptions.end())
        {
            while (vwap_sell_it != VWAP_subscriptions[symbol].end() && sell_volume_left > 0)
            {
                uint64_t tmp_volume = std::min(left_to_sell, sell_volume_left);
                vwap_sell[vwap_sell_res_ind] += last_sell_price * tmp_volume;
                left_to_sell -= tmp_volume;
                sell_volume_left -= tmp_volume;
                if (left_to_sell == 0)
                {
                    is_vwap_sell_complite[vwap_sell_res_ind] = true;
                    left_to_sell = *vwap_sell_it;
                    vwap_sell_it++;
                    vwap_sell_res_ind++;
                    if (vwap_sell_it != VWAP_subscriptions[symbol].end())
                    {
                        left_to_sell = *vwap_sell_it - left_to_sell;
                        vwap_sell[vwap_sell_res_ind] = vwap_sell[vwap_sell_res_ind - 1];
                    }
                }
            }
        }

        if (last_buy_price != 0)
        {
            std::cout << buy_volume << "@" << last_buy_price;
        }
        else
        {
            std::cout << "\t";
        }

        std::cout << "\t|\t";

        if (last_sell_price != 0)
        {
            std::cout << sell_volume << "@" << last_sell_price;
        }

        std::cout << "\n";
    }

    if (VWAP_subscriptions.find(symbol) != VWAP_subscriptions.end())
    {
        size_t subscription_ind = 0;
        for (auto subscription_it : VWAP_subscriptions[symbol])
        {
            std::cout << "VWAP " << symbol << " " << subscription_it;
            if (is_vwap_buy_complite[subscription_ind])
            {
                std::cout << "<" << vwap_buy[subscription_ind] / subscription_it << ", ";
            }
            else
            {
                std::cout << "<NIL, ";
            }
            if (is_vwap_sell_complite[subscription_ind])
            {
                std::cout << vwap_sell[subscription_ind] / subscription_it << ">\n";
            }
            else
            {
                std::cout << "NIL>\n";
            }
            subscription_ind++;
        }
    }
    
    std::cout << "\n";
}

void OrderBook::print_full(std::string const &symbol)
{
    std::cout << "Order book:\n";
    auto buy_order_it = orders_for_buy[symbol].begin();
    auto sell_order_it = orders_for_sell[symbol].rbegin();
    while (buy_order_it != orders_for_buy[symbol].end() || sell_order_it != orders_for_sell[symbol].rend())
    {
        double curr_price = 0;
        if (buy_order_it != orders_for_buy[symbol].end())
        {
            curr_price = (*buy_order_it)->price;
        }
        if (sell_order_it != orders_for_sell[symbol].rend())
        {
            curr_price = std::max(curr_price, (*sell_order_it)->price);
        }
        std::cout << "Price: " << curr_price << "\n";
        while (sell_order_it != orders_for_sell[symbol].rend() && (*sell_order_it)->price == curr_price)
        {
            std::cout << "Sell " << (*sell_order_it)->quantity << "@" << (*sell_order_it)->price << " " << (*sell_order_it)->symbol << "\n";
            sell_order_it++;
        }
        while (buy_order_it != orders_for_buy[symbol].end() && (*buy_order_it)->price == curr_price)
        {
            std::cout << "Buy " << (*buy_order_it)->quantity << "@" << (*buy_order_it)->price << " " << (*buy_order_it)->symbol << "\n";
            buy_order_it++;
        }
    }
    std::cout << "\n";
}

void OrderBook::subscribe_VWAP(std::string const &symbol, uint64_t quantity)
{
    if (!filters.empty() && filters.find("VWAP") == filters.end())
    {
        return;
    }
    VWAP_subscriptions[symbol].insert(quantity);
}

void OrderBook::unsubscribe_VWAP(std::string const &symbol, uint64_t quantity)
{
    if (!filters.empty() && filters.find("VWAP") == filters.end())
    {
        return;
    }
    if (VWAP_subscriptions.find(symbol) == VWAP_subscriptions.end() || VWAP_subscriptions[symbol].find(quantity) == VWAP_subscriptions[symbol].end())
    {
        throw std::invalid_argument("This subscribe does not exist");
    }
    VWAP_subscriptions[symbol].erase(VWAP_subscriptions[symbol].find(quantity));
    if (VWAP_subscriptions[symbol].empty())
    {
        VWAP_subscriptions.erase(symbol);
    }
}

void OrderBook::subscribe_BBO(std::string const &symbol)
{
    if (!filters.empty() && filters.find("BBO") == filters.end())
    {
        return;
    }
    BBO_subscriptions.insert(symbol);
}

void OrderBook::unsubscribe_BBO(std::string const &symbol)
{
    if (!filters.empty() && filters.find("BBO") == filters.end())
    {
        return;
    }
    BBO_subscriptions.erase(symbol);
}

bool cmp_for_buy::operator()(const std::shared_ptr<Order> &a, const std::shared_ptr<Order> &b)
{
    return a->price > b->price || (a->price == b->price && a->id < b->id);
}

bool cmp_for_sell::operator()(const std::shared_ptr<Order> &a, const std::shared_ptr<Order> &b)
{
    return a->price < b->price || (a->price == b->price && a->id < b->id);
}
