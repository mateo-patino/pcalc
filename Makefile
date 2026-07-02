# Set C standard according to compiler version
CC = gcc
USE_C23 := $(shell echo "int main(void) { return 0; }" | $(CC) -std=c23 -x c - -o /dev/null 2>/dev/null; echo $$?)

ifeq ($(USE_C23),0)
	CSTD = c23
else
	CSTD = c2x
endif

CFLAGS = -std=$(CSTD) -Wall -Wextra -Werror -pedantic-errors -g
CPPFLAGS = -Isrc

APP_TARGET = pcalc
TEST_TARGET = pcalc_tests
COMPILE_DB = compile_commands.json

SRC_DIR = src
TEST_DIR = tests
OBJ_DIR = build

APP_SRCS = $(wildcard $(SRC_DIR)/*.c)
APP_OBJS = $(APP_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
APP_OBJS_MAINLESS := $(filter-out $(OBJ_DIR)/main.o,$(APP_OBJS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)

COMPILE.c = $(CC) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC)

all: $(COMPILE_DB) $(APP_TARGET)
test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean test

# APP
$(APP_TARGET): $(APP_OBJS)
	$(LINK) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	# The -MMD and -MP flags create a .d file with the dependencies of build/%.o
	$(COMPILE.c) -MMD -MP -c $< -o $@

# TESTS
$(TEST_TARGET): $(TEST_OBJS) $(APP_OBJS_MAINLESS)
	$(LINK) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	# The -MMD and -MP flags create a .d file with the dependencies of build/%.o
	$(COMPILE.c) -MMD -MP -c $< -o $@

$(COMPILE_DB):
	# We close this brace block in the else block next iteration
	# Only the last object doesn't get closed here but outside the loop
	first=0; \
	echo "[" > $@; \
	for src in $(APP_SRCS) $(TEST_SRCS); do \
		if [ $$first -eq 0 ]; then \
			first=1; \
		else \
			echo "	}," >> $@; \
		fi; \
		echo "	{" >> $@; \
		echo "		\"directory\": \"$(CURDIR)\"," >> $@; \
		echo "		\"command\": \"$(COMPILE.c) -c $$src -o _\"," >> $@; \
		echo "		\"file\": \"$(CURDIR)/$$src\"" >> $@; \
	done; \
	echo "	}" >> $@; \
	echo "]" >> $@


clean:
	rm -rf $(OBJ_DIR) $(APP_TARGET) $(TEST_TARGET)

# This opens the .d file and the rules are merged with the one above.
-include $(APP_OBJS:.o=.d) $(TEST_OBJS:.o=.d)
