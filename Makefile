# This Makefile is used during development and testing of EpoxyDuino.
# Users of this library will use 'EpoxyDuino.mk' instead.

all:
	$(MAKE) -C libraries
	$(MAKE) -C examples

clean:
	$(MAKE) -C libraries clean
	$(MAKE) -C examples clean
