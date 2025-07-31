# إعدادات المترجم
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2
LDLIBS = -lncurses

# اسم الملف التنفيذي
TARGET = SnakeGame

# ملفات المصدر
SRCS = SnakeGame.cpp

# ملفات الكائن (تولد تلقائياً)
OBJS = $(SRCS:.cpp=.o)

# القواعد الأساسية
.PHONY: all clean rebuild run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

