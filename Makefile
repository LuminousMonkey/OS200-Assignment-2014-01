SRC = INCLUDES = -Isrc

CFLAGS = -g -MMD -Wall -pthread -pedantic -std=c99 -fstrict-aliasing $(INCLUDES)

LDFLAGS =

CC ?= gcc

.PHONY: clean dirs all

all: dirs roundrobin sjf simulator

dirs:
	@mkdir -p obj

SRCFILES := $(wildcard src/*.c)
OBJFILES := $(patsubst src/%.c,obj/%.o,$(SRCFILES))
DEPFILES := $(patsubst src/%.c,obj/%.d,$(SRCFILES))
OBJNOASSFILES := $(patsubst obj/simulator.o,,$(OBJFILES))

roundrobin: $(filter-out obj/sjf.o obj/simulator.o,$(OBJFILES))
	@echo [LD] $@
	@$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

sjf: $(filter-out obj/roundrobin.o obj/simulator.o,$(OBJFILES))
	@echo [LD] $@
	@$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

simulator: $(filter-out obj/roundrobin.o obj/sjf.o,$(OBJFILES))
	@echo [LD] $@
	@$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c
	@echo [CC] $@
	@$(CC) $(CFLAGS) -MF $(patsubst obj/%.o, obj/%.d,$@) -c $< -o $@

clean:
	rm -fr obj sjf roundrobin simulator

-include $(SRCFILES:.c=.d)
