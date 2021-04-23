/*
  EEPROM.h - EEPROM library
  Original Copyright (c) 2006 David A. Mellis.  All right reserved.
  New version by Christopher Andrews 2015.
  Substantially modified for EpoxyDuino by Brian T. Park 2019, 2021.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EPOXY_EEPROM_AVR_H
#define EPOXY_EEPROM_AVR_H

#include <inttypes.h>

/** Use this macro to distinguish between EpoxyEepromEsp or EpoxyEepromAvr. */
#define EPOXY_DUINO_EPOXY_EEPROM_AVR 1

/***
    EERef class.

    This object references an EEPROM cell.
    Its purpose is to mimic a typical byte of RAM, however its storage is the
    EEPROM. This class has an overhead of two bytes, similar to storing a
    pointer to an EEPROM cell.
***/

class EERef{
  public:
    EERef(int index, int fd)
      : index(index),
        fd(fd)
    {}

    //Access/read members.
    uint8_t operator*() const;
    operator uint8_t() const { return **this; }

    //Assignment/write members.
    EERef& operator=(const EERef &ref) { return *this = *ref; }
    EERef& operator=(uint8_t in);
    EERef& operator+=(uint8_t in) { index += in; return *this; }
    EERef& operator-=(uint8_t in) { index -= in; return *this; }
    EERef& operator*=(uint8_t in) { index *= in; return *this; }
    EERef& operator/=(uint8_t in) { index /= in; return *this; }
    EERef& operator^=(uint8_t in) { index ^= in; return *this; }
    EERef& operator%=(uint8_t in) { index %= in; return *this; }
    EERef& operator&=(uint8_t in) { index &= in; return *this; }
    EERef& operator|=(uint8_t in) { index |= in; return *this; }
    EERef& operator<<=(uint8_t in) { index <<= in; return *this; }
    EERef& operator>>=(uint8_t in) { index >>= in; return *this; }

    EERef &update(uint8_t in) {
      if (in != *this) *this = in;
      return *this;
    }

    /** Prefix increment/decrement **/
    EERef& operator++() { ++index; return *this; }
    EERef& operator--() { --index; return *this; }

    /** Postfix increment/decrement **/
    uint8_t operator++(int) {
      uint8_t ret = **this;
      index++;
      return ret;
    }

    uint8_t operator--(int) {
      uint8_t ret = **this;
      index--;
      return ret;
    }

  private:
    int index; //Index of current EEPROM cell.
    int fd; // file descriptor
};

/***
    EEPtr class.

    This object is a bidirectional pointer to EEPROM cells represented by EERef
    objects. Just like a normal pointer type, this can be dereferenced and
    repositioned using increment/decrement operators.
***/

class EEPtr{
  public:
    EEPtr(int index, int fd)
      : index(index),
        fd(fd)
    {}

    operator int() const { return index; }
    EEPtr &operator=(int in) { index = in; return *this; }

    //Iterator functionality.
    bool operator!=(const EEPtr &ptr) { return index != ptr.index; }
    EERef operator*() { return EERef(index, fd); }

    /** Prefix & Postfix increment/decrement **/
    EEPtr& operator++() { ++index; return *this; }
    EEPtr& operator--() { --index; return *this; }
    EEPtr operator++(int) { return EEPtr(index++, fd); }
    EEPtr operator--(int) { return EEPtr(index--, fd); }

  private:
    int index; //Index of current EEPROM cell.
    int fd; // file descriptor
};

/***
    EpoxyEepromAvr class.

    This object represents the entire EEPROM space.
    It wraps the functionality of EEPtr and EERef into a basic interface.
    This class is also 100% backwards compatible with earlier Arduino core
    releases.
***/

class EpoxyEepromAvr{
  public:
    EpoxyEepromAvr();
    ~EpoxyEepromAvr();

    //Basic user access methods.
    EERef operator[](int idx) const { return EERef(idx, fd); }

    uint8_t read(int idx) const { return EERef(idx, fd); }

    void write(int idx, uint8_t val) { (EERef(idx, fd)) = val; }

    void update(int idx, uint8_t val) { EERef(idx, fd).update(val); }

    //STL and C++11 iteration capability.
    EEPtr begin() const { return EEPtr(0x00, fd); }

    //Standards requires this to be the item after the last valid entry. The
    //returned pointer is invalid.
    EEPtr end() const { return EEPtr(length(), fd); }

    uint16_t length() const { return kEpoxyEepromSize; }

    //Functionality to 'get' and 'put' objects to and from EEPROM.
    template <typename T>
    T &get(int idx, T &t) const {
      EEPtr e(idx, fd);
      uint8_t *ptr = (uint8_t*) &t;
      for (int count = sizeof(T) ; count ; --count, ++e) {
        *ptr++ = *e;
      }
      return t;
    }

    template <typename T>
    const T &put(int idx, const T &t) {
      EEPtr e(idx, fd);
      const uint8_t *ptr = (const uint8_t*) &t;
      for (int count = sizeof(T) ; count ; --count, ++e ) {
        (*e).update(*ptr++);
      }
      return t;
    }

  private:
    static const char* getDataPath();
    static const uint16_t kEpoxyEepromSize = 1024;

    int fd;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
extern EpoxyEepromAvr EpoxyEepromAvrInstance;
#endif

#endif
