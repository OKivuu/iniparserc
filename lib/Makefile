#BUILD_MODE := debug

PATH_SRC := ./src
PATH_OBJ := ./obj
PATH_BIN := ./bin

CC := gcc
LIBER := ar

space :=
space +=
vpath %.c $(shell find . -type d -iname 'src' -not -path "*nosources/*" -not -path "*nosourcesother/*")

FILES_LIST = \
iniparserc \

SRCS := $(addsuffix .c, $(FILES_LIST))
OBJS := $(addsuffix .o, $(FILES_LIST))
OBJS := $(addprefix $(PATH_OBJ)/, $(OBJS))

CFLAGS = -c

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g3
else
	CFLAGS += -O2
	#$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

DEPFLAGS = -MT $@ -MMD -MF $(PATH_OBJ)/$*.Td
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS)
POSTPROCESS = @sed -e 's,\([A-Za-z]*\):/,/\1/,g' $(PATH_OBJ)/$*.Td > $(PATH_OBJ)/$*.d
POSTCOMPILE = @rm $(PATH_OBJ)/$*.Td && touch $@
OUTPUT_OPTION = -o $@

$(PATH_OBJ)/%.d: ;
.PRECIOUS: $(PATH_OBJ)/%.d
-include $(OBJS:.o=.d)

all:	dirs iniparserc

dirs:
	mkdir -p $(PATH_OBJ)
	mkdir -p $(PATH_BIN)

iniparserc:	$(OBJS)
	@echo "Generating..."
	$(LIBER) -c -r $(PATH_BIN)/$@.a $^ 
	@echo " OK!"

$(PATH_OBJ)/%.o : %.c
$(PATH_OBJ)/%.o : %.c $(PATH_OBJ)/%.d
	@echo -n "$< ..."
	@$(COMPILE.c) $(OUTPUT_OPTION) $<
	@$(POSTPROCESS)
	@$(POSTCOMPILE)
	@echo " DONE!"

clean:
	rm -fr iniparserc_test $(OBJS)
