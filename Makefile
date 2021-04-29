CXX=g++

CXXFLAGS=-c -Wall -Wextra

all: md_reply

md_reply: md_replay.o Order.o OrderBook.o MarketDataParser.o
	$(CXX) md_replay.o Order.o OrderBook.o MarketDataParser.o -o md_replay

md_replay.o: md_replay.cpp
	$(CXX) $(CXXFLAGS) md_replay.cpp

Order.o: Order.cpp
	$(CXX) $(CXXFLAGS) Order.cpp

OrderBook.o: OrderBook.cpp
	$(CXX) $(CXXFLAGS) OrderBook.cpp

MarketDataParser.o: MarketDataParser.cpp
	$(CXX) $(CXXFLAGS) MarketDataParser.cpp

clean:
	rm -rf *.o md_replay
