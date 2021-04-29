/*
  Copyright (c) Brian T. Park 2021.

  Persistence code inspired by MockEEPROM.cpp from the ESP8266 Arduino Core.

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

#include <stdlib.h> // getenv()
#include <sys/types.h> // open()
#include <sys/stat.h> // S_IRUSR, S_IWUSR, ...
#include <unistd.h> // pread(), pwrite()
#include <fcntl.h> // O_CREAT, etc (I think)
#include "EpoxyEepromAvr.h"

EpoxyEepromAvr::EpoxyEepromAvr() {
  const char* dataPath = getDataPath();
  fd = open(
      dataPath,
      O_CREAT | O_RDWR,
      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
  );
  if (fd != -1) {
    int status = ftruncate(fd, length());
    if (status == -1) {
      fd = -1;
    }
  }
}

EpoxyEepromAvr::~EpoxyEepromAvr() {
  if (fd != -1) close(fd);
}

const char* EpoxyEepromAvr::getDataPath() {
  const char* dataPath = getenv("EPOXY_EEPROM_DATA");
  if (!dataPath) {
    dataPath = "epoxyeepromdata";
  }
  return dataPath;
}

uint8_t EERef::operator*() const {
	char c = 0;
  // Ignore errors because the EEPROM API has no mechanism for dealing them.
	pread(fd, &c, 1, index);
  return c;
}

EERef& EERef::operator=(uint8_t in) {
  // Ignore errors because the EEPROM API has no mechanism for dealing them.
  pwrite(fd, &in, 1, index);
  return  *this;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EpoxyEepromAvr EpoxyEepromAvrInstance;
#endif
