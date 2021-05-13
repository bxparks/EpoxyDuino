/*
 * MIT License
 * Copyright (c) 2021 Brian T. Park
 */

/**
 * @file Simple mocking library for the digitalWriteFast library
 * (https://github.com/NicksonYap/digitalWriteFast) to allow code written
 * against that library to compile under EpoxyDuino.
 */

#ifndef EPOXY_MOCK_DIGITAL_WRITE_FAST_H
#define EPOXY_MOCK_DIGITAL_WRITE_FAST_H

#include <stdint.h> // uint8_t

inline void digitalWriteFast(uint8_t /*pin*/, uint8_t /*value*/) {}

inline void pinModeFast(uint8_t /*pin*/, uint8_t /*value*/) {}

inline uint8_t digitalReadFast(uint8_t /*pin*/) { return 0; }

#endif
