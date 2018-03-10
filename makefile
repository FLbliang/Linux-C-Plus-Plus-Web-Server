SRC = $(wildcard *.cpp)
O_FILE = $(patsubst %.cpp, %.o, $(SRC))
TARGET = web_server
FLAGS = -std=c++11 -pthread
CPP = g++

$(TARGET):$(O_FILE)
	$(CPP) $(O_FILE) -o $@ $(FLAGS)

%.o:%.cpp
	$(CPP) $< -c $(FLAGS)

.PHONY:
clean:
	rm $(TARGET) -f
