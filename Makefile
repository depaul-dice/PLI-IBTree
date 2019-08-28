SOURCEDIR := ibtree
BUILDDIR := build
TARGET := build
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
SOURCES := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.cc)
LIBS= -pthread -L /usr/lib/postgresql/9.6/lib -I/usr/include/postgresql/libpq -lpqxx -lpq
CXX=g++


all: $(SOURCES)
	$(CXX) -std=c++11 -o $@ $^ $(LIBS)


