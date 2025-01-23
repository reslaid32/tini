# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -fPIC
LDFLAGS = -shared
LDFLAGS_TEST =

# Directories
BIN = bin
SRC = src
INC = include
OBJ = $(BIN)/tmp

DESTDIR = /usr/
LIBDIR = $(DESTDIR)/lib
INCLUDEDIR = $(DESTDIR)/include
TEST_SRC = tests
TEST_BIN = $(BIN)/tests
# BINDIR = $(DESTDIR)/bin

# Targets
TARGET_LIB = $(BIN)/libtini.so
INSTALL_LIB = $(LIBDIR)/libtini.so
INSTALL_INC = $(INCLUDEDIR)/tini.h

# Source files
SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

# Test files
TEST_SRCS = $(wildcard $(TEST_SRC)/*.c)
TEST_OBJS = $(patsubst $(TEST_SRC)/%.c,$(OBJ)/%.o,$(TEST_SRCS))
TEST_EXEC = $(TEST_BIN)/unit_tests

# Default rule: Build the dynamic library
all: $(TARGET_LIB)

# Build the shared library
$(TARGET_LIB): $(OBJS)
	mkdir -p $(BIN)
	$(CC) $(LDFLAGS) -I$(INC) -o $@ $^

# Compile object files
$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

# Compile test object files
$(OBJ)/%.o: $(TEST_SRC)/%.c
	mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@ -g

# Build the test executable
$(TEST_EXEC): $(TEST_OBJS) $(TARGET_LIB)
	mkdir -p $(TEST_BIN)
	$(CC) $(LDFLAGS_TEST) -I$(INC) -o $@ $^ -L$(BIN) -ltini -g

# Clean up garbage (objs)
clean-garbage:
	rm -rf $(OBJ)

# Clean up build artifacts
clean:
	rm -rf $(OBJ) $(TARGET_LIB)

# Install the library and header file
install: $(TARGET_LIB)
	# Install the shared library
	install -d $(LIBDIR)
	install -m 755 $(TARGET_LIB) $(INSTALL_LIB)

	# Install the header file
	install -d $(INCLUDEDIR)
	install -m 644 $(INC)/tini.h $(INSTALL_INC)

	@echo "Installed libtini.so to $(LIBDIR) and tini.h to $(INCLUDEDIR)"

# Uninstall the library and header file
uninstall:
	# Remove the shared library
	rm -f $(INSTALL_LIB)

	# Remove the header file
	rm -f $(INSTALL_INC)

	@echo "Uninstalled libtini.so and tini.h"

check: $(TEST_EXEC)
	$(TEST_EXEC)

# Phony targets
.PHONY: all clean clean-garbage install uninstall check
