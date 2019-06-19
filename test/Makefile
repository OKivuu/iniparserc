#BUILD_MODE := debug

PATH_SRC := ./src
PATH_OBJ := ./obj
PATH_BIN := ./bin

CC := gcc

vpath %.c $(PATH_SRC)

INCLUDES_PATH = \
-I ../lib/inc \

LIB_PATH = \
../lib/bin/iniparserc.a \

FILES_LIST = \
main \

SRCS := $(addsuffix .c, $(FILES_LIST))
OBJS := $(addsuffix .o, $(FILES_LIST))
OBJS := $(addprefix $(PATH_OBJ)/, $(OBJS))

CFLAGS = -c $(INCLUDES_PATH)

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g3
else
	CFLAGS += -O2
	#$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	dirs iniparserc_test

dirs:
	mkdir -p $(PATH_OBJ)
	mkdir -p $(PATH_BIN)

iniparserc_test:	$(OBJS) $(LIB_PATH) 
	@echo "Generating..."
	$(CC) -o $(PATH_BIN)/$@ $^ 
	@echo " OK!"

$(PATH_OBJ)/%.o:	$(PATH_SRC)/%.c
	@echo -n "$< ..."
	$(CC) $(CFLAGS) -o $@ $<
	@echo " DONE!"

clean:
	rm -fr iniparserc_test $(OBJS)
