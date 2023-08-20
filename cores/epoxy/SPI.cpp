/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * Copyright (c) 2021 by Erik Tideman (Replaced implementations with stubs.)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"

/*Read and write buffers for mocking. 16 bits to support transfer16*/
#define BUFFERSIZE 4
uint16_t readBuffer = 0x0000;
uint16_t writeBuffer[BUFFERSIZE];
uint8_t callCount = 0;

SPIClass SPI;

void SPIClass::end() {}

void SPIClass::usingInterrupt(uint8_t /*interruptNumber*/) {}

void SPIClass::notUsingInterrupt(uint8_t /*interruptNumber*/) {}

uint16_t SPIClass::transfer16(uint16_t data)
{
	addToBuffer(data);
	callCount++;
	return readBuffer;
}

uint8_t SPIClass::transfer(uint8_t data)
{

	addToBuffer(data);
	callCount++;
	return (uint8_t)readBuffer;
}

void SPIClass::transferReturnValue(uint16_t data)
{
	readBuffer = data;
}

/*Number of calls previous written value*/
uint16_t SPIClass::transferWriteValue(uint8_t n)
{
	return writeBuffer[((n-1)%BUFFERSIZE)];
}

void SPIClass::resetWriteBuffer()
{
	memset(writeBuffer, 0, sizeof(uint16_t)*BUFFERSIZE);
	callCount = 0;
}

void SPIClass::addToBuffer(uint16_t data)
{
	/*clear write buffer*/
	writeBuffer[callCount%BUFFERSIZE] = 0x0000;
	/*set lower byte to transferred value*/
	writeBuffer[callCount%BUFFERSIZE] |= data;
}

uint8_t SPIClass::getCallCount(){
	return callCount;
}
