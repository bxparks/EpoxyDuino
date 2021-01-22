# See https://github.com/bxparks/UnixHostDuino for documentation about this
# Makefile to compile and run Arduino programs natively on Linux or MacOS.

APP_NAME := UnixHostFSTest
ARDUINO_LIBS := AUnit AceCommon UnixHostFS
MORE_CLEAN := more_clean
include $(HOME)/src/UnixHostDuino/UnixHostDuino.mk

more_clean:
	rm -rf fsdata
