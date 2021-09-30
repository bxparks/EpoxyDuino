# This Makefile is used during development and testing of EpoxyDuino.
# Users of this library will use 'EpoxyDuino.mk' instead.

all:
	$(MAKE) -C tests
	$(MAKE) -C libraries
	$(MAKE) -C examples

runtests:
	$(MAKE) -C tests runtests
	$(MAKE) -C libraries runtests

clean:
	$(MAKE) -C tests clean
	$(MAKE) -C libraries clean
	$(MAKE) -C examples clean
