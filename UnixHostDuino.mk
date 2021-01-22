# This file provides backwards compatibility.
EPOXY_DUINO_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
include $(EPOXY_DUINO_DIR)/EpoxyDuino.mk
