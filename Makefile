CONFIG_MODULE_SIG = n
TARGET_MODULE := fibdrv

obj-m += $(TARGET_MODULE).o
fibdrv-objs := \
	src/fibdrv.o \
	src/bignum.o \
	src/algorithm.o
	
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied
FILE ?= 4

all: $(GIT_HOOKS) client plotsrc
	$(MAKE) -C $(KDIR) M=$(PWD) modules 

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) client out plotsrc
	$(RM) plot/*
	$(RM) record/*
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true >/dev/null

client: src/client.c
	$(CC) -o $@ $^

plotsrc: src/plot.c
	$(CC) -o $@ $^


PRINTF = env printf
PASS_COLOR = \e[32;01m
NO_COLOR = \e[0m
pass = $(PRINTF) "$(PASS_COLOR)$1 Passed [-]$(NO_COLOR)\n"

plot: all
	$(MAKE) unload
	$(MAKE) load
	@python3 scripts/driver.py $(FILE)
	$(MAKE) unload



cmpplot: all
	$(MAKE) unload
	$(MAKE) load
	@python3 scripts/driver.py $(FILE)
	@python3 scripts/cmp_driver.py $(FILE)
	$(MAKE) unload

check: all
	$(MAKE) unload
	$(MAKE) load
	sudo ./client $(FILE) > out
	$(MAKE) unload
	@diff -u out scripts/expected.txt && $(call pass)
	@scripts/verify.py

help : 					## Show help message
	@printf "\n====== fibdrv algorithm ID ======\n\n"
	@printf "ID : 0	-> original_iter\n"
	@printf "ID : 1	-> fdoubling_rec\n"
	@printf "ID : 2	-> fdoubling_iter\n"
	@printf "ID : 3	-> bn_fib\n"
	@printf "ID : 4	-> bn_fib_fdoubling\n"
	@printf "ID : 5	-> bn_fib_fdoubling_v1\n\n"
	@printf "You can type ' make plot FILE=ID' and, \n"
	@printf "get the algorithm performance fig.\n"
	@printf "=================================\n"

