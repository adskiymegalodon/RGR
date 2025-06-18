# Компилятор и базовые флаги
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC
LDFLAGS := -L.
LDLIBS := 

# Определение операционной системы
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    CXXFLAGS += -DLINUX
    LDLIBS += -ldl
    DYNAMIC_EXT := .so
    DYNAMIC_FLAGS := -shared
    STATIC_EXT := .a
else ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -DMACOS
    LDLIBS += -ldl
    DYNAMIC_EXT := .dylib
    DYNAMIC_FLAGS := -dynamiclib
    STATIC_EXT := .a
else
    CXXFLAGS += -DWINDOWS
    LDLIBS += -luser32 -lkernel32
    DYNAMIC_EXT := .dll
    DYNAMIC_FLAGS := -shared
    STATIC_EXT := .lib
endif

# Целевые файлы
TARGET := unreal_rgr
DYNAMIC_LIB := libcipher$(DYNAMIC_EXT)
STATIC_LIB := libcipher$(STATIC_EXT)  # Добавлено имя статической библиотеки

# Списки исходных файлов
MAIN_SRCS := main.cpp ui_utilities.cpp installer.cpp file_operations.cpp
CIPHER_SRCS := cipher.cpp atbash.cpp vigenere.cpp table_transposition.cpp

MAIN_OBJS := $(MAIN_SRCS:.cpp=.o)
CIPHER_OBJS := $(CIPHER_SRCS:.cpp=.o)

# Правила сборки
.PHONY: all clean lib static

all: $(TARGET) lib static  # Теперь включает обе библиотеки

# Основная программа
$(TARGET): $(MAIN_OBJS) $(CIPHER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Правило для динамической библиотеки
$(DYNAMIC_LIB): $(CIPHER_OBJS)
	$(CXX) $(CXXFLAGS) $(DYNAMIC_FLAGS) -o $@ $^ $(LDLIBS)

# Правило для статической библиотеки (ДОБАВЛЕНО)
$(STATIC_LIB): $(CIPHER_OBJS)
	ar rcs $@ $^

# Явные цели для библиотек
lib: $(DYNAMIC_LIB)
static: $(STATIC_LIB)  # Добавлена цель для статической библиотеки

# Общее правило для объектных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f *.o $(TARGET) $(DYNAMIC_LIB) $(STATIC_LIB) *.dll *.so *.dylib *.a