#include <iostream>
#include "MarketDataParser.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Invalid argument";
        return 0;
    }
    string file_path = argv[1];
    unordered_set<string> filters;
    for (int i = 2; i < argc; i++)
    {
        filters.insert(argv[i]);
    }
    try
    {
        MarketDataParser marketDataParser(file_path, filters);
        marketDataParser.read_file();
    }
    catch (exception &e)
    {
        cout << e.what();
    }


    return 0;
}
