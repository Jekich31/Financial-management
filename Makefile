CXX = g++
CXXFLAGS = -std=c++17 -Wall

TARGET = finance_app
SOURCES = main.cpp core.cpp menu.cpp utils.cpp
HEADERS = types.h models.h core.h menu.h utils.h

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
