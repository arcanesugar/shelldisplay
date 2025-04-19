CC:=clang
CFLAGS:=-MMD -MP -O3 -lz
BINNAME:=main

SRC:= main.c shelldisplay.c
OBJ:=$(SRC:.c=.o)
DEP:=$(SRC:.c=.d)

# $@ = name of the current target
# @^ = current targets dependencies
# $@:$^
# .PHONY means the rule does not refer to a file

all:$(BINNAME)

$(BINNAME):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lm

%.o:%.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEP)

.PHONY:clean
clean:
	@rm -f $(OBJ)
	@rm -f $(DEP)
	@rm -f $(BINNAME)
