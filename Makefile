# Makefile for weather_app

CC = g++
CFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lcurl
SRC = weather_app.cpp
TARGET = weather_app

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

