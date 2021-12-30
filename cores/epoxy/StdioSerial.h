/*
 * Copyright (c) 2019 Brian T. Park
 * MIT License
 */

#ifndef EPOXY_DUINO_STDIO_SERIAL_H
#define EPOXY_DUINO_STDIO_SERIAL_H

#include "Print.h"
#include "Stream.h"

/**
 * A version of Serial that reads from STDIN and sends output to STDOUT on
 * Linux or MacOS.
 */
class StdioSerial: public Stream {
  public:
    void begin(unsigned long /*baud*/) { bufch = -1; }

    size_t write(uint8_t c) override;

    void flush() override;

    operator bool() { return true; }

    int available() override;

    int read() override;
	
    int peek() override;

    
  private:
    int bufch;
};

extern StdioSerial Serial;

#define SERIAL_PORT_MONITOR Serial

#endif
