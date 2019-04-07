# Copyright (c) 2017 Team Swipp
# Distributed under the MIT/X11 software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

OBJDIR = obj
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, $(OBJDIR)/%.o, \
	$(SRCS)) $(filter-out ../obj/bitcoind.o, $(wildcard ../obj/*.o)) $(wildcard ../obj/crypto/*.o)
DEFS += $(addprefix -I,$(CURDIR)/../leveldb/include)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -pthread -I.. $(DEFS)

LIBS += -Wl,-Bdynamic \
	-l boost_system$(BOOST_LIB_SUFFIX) \
	-l boost_filesystem$(BOOST_LIB_SUFFIX) \
	-l boost_program_options$(BOOST_LIB_SUFFIX) \
	-l boost_thread$(BOOST_LIB_SUFFIX) \
	-l boost_unit_test_framework$(BOOST_LIB_SUFFIX) \
	-l db_cxx$(BDB_LIB_SUFFIX) \
	-l ssl \
	-l crypto \
	-l pthread \
	-l z \
	-l dl \
	-l miniupnpc \
	-l curl \
	$(CURDIR)/../leveldb/out-static/libleveldb.a \
	$(CURDIR)/../leveldb/out-static/libmemenv.a

check: tests
	./tests

tests: $(OBJS)
	$(CXX) $(output) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJDIR)/*.o

.PHONY: all clean
