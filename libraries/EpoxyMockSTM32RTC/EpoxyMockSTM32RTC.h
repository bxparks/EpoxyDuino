#ifndef EPOXY_MOCK_STM32RTC_H
#define EPOXY_MOCK_STM32RTC_H

#include <stdint.h> // uint8_t

//-----------------------------------------------------------------------------
// Typedefs from STM32RTC/rtc.h
//-----------------------------------------------------------------------------

typedef enum {
  HOUR_FORMAT_12,
  HOUR_FORMAT_24
} hourFormat_t;

typedef enum {
  HOUR_AM,
  HOUR_PM
} hourAM_PM_t;

//-----------------------------------------------------------------------------
// Typedefs from cores/arduino/stm32/clock.h
//-----------------------------------------------------------------------------

typedef enum {
  LSI_CLOCK,
  HSI_CLOCK,
  LSE_CLOCK,
  HSE_CLOCK
} sourceClock_t;

//-----------------------------------------------------------------------------

/**
 * A mock version of the STM32RTC class in the
 * https://github.com/stm32duino/STM32RTC library. Only the clock-related
 * functions are mocked. The alarm-related functions are ignored.
 */
class STM32RTC {
  public:

    enum Hour_Format : uint8_t {
      HOUR_12 = HOUR_FORMAT_12,
      HOUR_24 = HOUR_FORMAT_24
    };

    enum AM_PM : uint8_t {
      AM = HOUR_AM,
      PM = HOUR_PM
    };

    enum Source_Clock : uint8_t {
      LSI_CLOCK = ::LSI_CLOCK,
      LSE_CLOCK = ::LSE_CLOCK,
      HSE_CLOCK = ::HSE_CLOCK
    };

    static STM32RTC &getInstance() {
      static STM32RTC instance; // Guaranteed to be destroyed.
      // Instantiated on first use.
      return instance;
    }

    STM32RTC(STM32RTC const &)        = delete;
    void operator=(STM32RTC const &)  = delete;

    void begin(bool resetTime, Hour_Format format = HOUR_24) {
      (void) resetTime;
      (void) format;
    }
    void begin(Hour_Format format = HOUR_24) {
      (void) format;
    }

    void end(void) {}

    Source_Clock getClockSource(void) { return _clockSource; }
    void setClockSource(Source_Clock source) { _clockSource = source; }

    /* Get Functions */

    uint32_t getSubSeconds(void) { return 0; }
    uint8_t getSeconds(void) { return 0; }
    uint8_t getMinutes(void) { return 0; }
    uint8_t getHours(AM_PM *period = nullptr) {
      (void) period;
      return 0;
    }
    void getTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds,
        uint32_t *subSeconds, AM_PM *period = nullptr) {
      (void) hours;
      (void) minutes;
      (void) seconds;
      (void) subSeconds;
      (void) period;
    }

    uint8_t getWeekDay(void) { return 1; }
    uint8_t getDay(void) { return 1; }
    uint8_t getMonth(void) { return 1; }
    uint8_t getYear(void) { return 0; }
    void getDate(
        uint8_t *weekDay, uint8_t *day, uint8_t *month, uint8_t *year) {
      (void) weekDay;
      (void) day;
      (void) month;
      (void) year;
    }

    /* Set Functions */

    void setSubSeconds(uint32_t subSeconds) { (void) subSeconds; }
    void setSeconds(uint8_t seconds) { (void) seconds; }
    void setMinutes(uint8_t minutes) { (void) minutes; }
    void setHours(uint8_t hours, AM_PM period = AM) {
      (void) hours;
      (void) period;
    }
    void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds,
        uint32_t subSeconds = 1000, AM_PM period = AM) {
      (void) hours;
      (void) minutes;
      (void) seconds;
      (void) subSeconds;
      (void) period;
    }

    void setWeekDay(uint8_t weekDay) { (void) weekDay; }
    void setDay(uint8_t day) { (void) day; }
    void setMonth(uint8_t month) { (void) month; }
    void setYear(uint8_t year) { (void) year; }
    void setDate(uint8_t day, uint8_t month, uint8_t year) {
      (void) day;
      (void) month;
      (void) year;
    }
    void setDate(uint8_t weekDay, uint8_t day, uint8_t month, uint8_t year) {
      (void) weekDay;
      (void) day;
      (void) month;
      (void) year;
    }

    bool isTimeSet(void) { return true; }

  private:
    STM32RTC(void): _clockSource(LSI_CLOCK) {}

    Source_Clock _clockSource;
};

#endif
