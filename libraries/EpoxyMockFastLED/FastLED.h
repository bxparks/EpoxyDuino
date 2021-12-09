/*
 * MIT License
 * Copyright (c) 2013 FastLED
 * Copyright (c) 2021 Brian T. Park
 */

/**
 * @file Simple mock implementation of the FastLED library
 * (https://github.com/FastLED/FastLED) to allow code written against
 * that library to compile under EpoxyDuino.
 */

#ifndef EPOXY_MOCK_FAST_LED_H
#define EPOXY_MOCK_FAST_LED_H

#include <stdint.h> // uint8_t

//-----------------------------------------------------------------------------
// From FastLED/src/led_sysdefs.h
//-----------------------------------------------------------------------------

#define FASTLED_USING_NAMESPACE

//-----------------------------------------------------------------------------
// From FastLED/src/pixeltypes.h
//-----------------------------------------------------------------------------

struct CHSV {
  union {
    struct {
      union {
        uint8_t hue;
        uint8_t h;
      };
      union {
        uint8_t saturation;
        uint8_t sat;
        uint8_t s;
      };
      union {
        uint8_t value;
        uint8_t val;
        uint8_t v;
      };
    };
    uint8_t raw[3];
  };

  // constructor
  CHSV(uint8_t ih, uint8_t is, uint8_t iv)
      : h(ih), s(is), v(iv)
  {}

  // copy constructor
  CHSV(const CHSV& rhs) = default;

  // assignment operator
  CHSV& operator=(const CHSV& rhs) = default;
};

struct CRGB {
  union {
    struct {
      union {
        uint8_t r;
        uint8_t red;
      };
      union {
        uint8_t g;
        uint8_t green;
      };
      union {
        uint8_t b;
        uint8_t blue;
      };
    };
    uint8_t raw[3];
  };

  CRGB() {
    r = g = b = 0;
  }

  // constructor
  CRGB(uint8_t ir, uint8_t ig, uint8_t ib)
      : r(ir), g(ig), b(ib)
  {}

  // copy constructor
  CRGB(const CRGB& rhs) = default;

  CRGB(uint32_t colorcode) :
      r((colorcode >> 16) & 0xFF),
      g((colorcode >> 8) & 0xFF),
      b((colorcode >> 0) & 0xFF)
  {}

  // assignment operator
  CRGB& operator= (const CRGB& rhs) = default;

  /// allow construction from HSV color
  CRGB(const CHSV& rhs) {
    // This does not perform the correct conversion. Defined only for mocking.
    r = rhs.h;
    g = rhs.s;
    b = rhs.v;
  }

  CRGB& operator+= (const CRGB& rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    return *this;
  }

  CRGB& operator%= (uint8_t scaledown) {
    (void) scaledown;
    return *this;
  }

  CRGB& operator|= (const CRGB& rhs) {
    if (rhs.r > r) r = rhs.r;
    if (rhs.g > g) g = rhs.g;
    if (rhs.b > b) b = rhs.b;
    return *this;
  }

  /// Predefined RGB colors
  typedef enum {
    AliceBlue=0xF0F8FF,
    Amethyst=0x9966CC,
    AntiqueWhite=0xFAEBD7,
    Aqua=0x00FFFF,
    Aquamarine=0x7FFFD4,
    Azure=0xF0FFFF,
    Beige=0xF5F5DC,
    Bisque=0xFFE4C4,
    Black=0x000000,
    BlanchedAlmond=0xFFEBCD,
    Blue=0x0000FF,
    BlueViolet=0x8A2BE2,
    Brown=0xA52A2A,
    BurlyWood=0xDEB887,
    CadetBlue=0x5F9EA0,
    Chartreuse=0x7FFF00,
    Chocolate=0xD2691E,
    Coral=0xFF7F50,
    CornflowerBlue=0x6495ED,
    Cornsilk=0xFFF8DC,
    Crimson=0xDC143C,
    Cyan=0x00FFFF,
    DarkBlue=0x00008B,
    DarkCyan=0x008B8B,
    DarkGoldenrod=0xB8860B,
    DarkGray=0xA9A9A9,
    DarkGrey=0xA9A9A9,
    DarkGreen=0x006400,
    DarkKhaki=0xBDB76B,
    DarkMagenta=0x8B008B,
    DarkOliveGreen=0x556B2F,
    DarkOrange=0xFF8C00,
    DarkOrchid=0x9932CC,
    DarkRed=0x8B0000,
    DarkSalmon=0xE9967A,
    DarkSeaGreen=0x8FBC8F,
    DarkSlateBlue=0x483D8B,
    DarkSlateGray=0x2F4F4F,
    DarkSlateGrey=0x2F4F4F,
    DarkTurquoise=0x00CED1,
    DarkViolet=0x9400D3,
    DeepPink=0xFF1493,
    DeepSkyBlue=0x00BFFF,
    DimGray=0x696969,
    DimGrey=0x696969,
    DodgerBlue=0x1E90FF,
    FireBrick=0xB22222,
    FloralWhite=0xFFFAF0,
    ForestGreen=0x228B22,
    Fuchsia=0xFF00FF,
    Gainsboro=0xDCDCDC,
    GhostWhite=0xF8F8FF,
    Gold=0xFFD700,
    Goldenrod=0xDAA520,
    Gray=0x808080,
    Grey=0x808080,
    Green=0x008000,
    GreenYellow=0xADFF2F,
    Honeydew=0xF0FFF0,
    HotPink=0xFF69B4,
    IndianRed=0xCD5C5C,
    Indigo=0x4B0082,
    Ivory=0xFFFFF0,
    Khaki=0xF0E68C,
    Lavender=0xE6E6FA,
    LavenderBlush=0xFFF0F5,
    LawnGreen=0x7CFC00,
    LemonChiffon=0xFFFACD,
    LightBlue=0xADD8E6,
    LightCoral=0xF08080,
    LightCyan=0xE0FFFF,
    LightGoldenrodYellow=0xFAFAD2,
    LightGreen=0x90EE90,
    LightGrey=0xD3D3D3,
    LightPink=0xFFB6C1,
    LightSalmon=0xFFA07A,
    LightSeaGreen=0x20B2AA,
    LightSkyBlue=0x87CEFA,
    LightSlateGray=0x778899,
    LightSlateGrey=0x778899,
    LightSteelBlue=0xB0C4DE,
    LightYellow=0xFFFFE0,
    Lime=0x00FF00,
    LimeGreen=0x32CD32,
    Linen=0xFAF0E6,
    Magenta=0xFF00FF,
    Maroon=0x800000,
    MediumAquamarine=0x66CDAA,
    MediumBlue=0x0000CD,
    MediumOrchid=0xBA55D3,
    MediumPurple=0x9370DB,
    MediumSeaGreen=0x3CB371,
    MediumSlateBlue=0x7B68EE,
    MediumSpringGreen=0x00FA9A,
    MediumTurquoise=0x48D1CC,
    MediumVioletRed=0xC71585,
    MidnightBlue=0x191970,
    MintCream=0xF5FFFA,
    MistyRose=0xFFE4E1,
    Moccasin=0xFFE4B5,
    NavajoWhite=0xFFDEAD,
    Navy=0x000080,
    OldLace=0xFDF5E6,
    Olive=0x808000,
    OliveDrab=0x6B8E23,
    Orange=0xFFA500,
    OrangeRed=0xFF4500,
    Orchid=0xDA70D6,
    PaleGoldenrod=0xEEE8AA,
    PaleGreen=0x98FB98,
    PaleTurquoise=0xAFEEEE,
    PaleVioletRed=0xDB7093,
    PapayaWhip=0xFFEFD5,
    PeachPuff=0xFFDAB9,
    Peru=0xCD853F,
    Pink=0xFFC0CB,
    Plaid=0xCC5533,
    Plum=0xDDA0DD,
    PowderBlue=0xB0E0E6,
    Purple=0x800080,
    Red=0xFF0000,
    RosyBrown=0xBC8F8F,
    RoyalBlue=0x4169E1,
    SaddleBrown=0x8B4513,
    Salmon=0xFA8072,
    SandyBrown=0xF4A460,
    SeaGreen=0x2E8B57,
    Seashell=0xFFF5EE,
    Sienna=0xA0522D,
    Silver=0xC0C0C0,
    SkyBlue=0x87CEEB,
    SlateBlue=0x6A5ACD,
    SlateGray=0x708090,
    SlateGrey=0x708090,
    Snow=0xFFFAFA,
    SpringGreen=0x00FF7F,
    SteelBlue=0x4682B4,
    Tan=0xD2B48C,
    Teal=0x008080,
    Thistle=0xD8BFD8,
    Tomato=0xFF6347,
    Turquoise=0x40E0D0,
    Violet=0xEE82EE,
    Wheat=0xF5DEB3,
    White=0xFFFFFF,
    WhiteSmoke=0xF5F5F5,
    Yellow=0xFFFF00,
    YellowGreen=0x9ACD32,

    // LED RGB color that roughly approximates
    // the color of incandescent fairy lights,
    // assuming that you're using FastLED
    // color correction on your LEDs (recommended).
    FairyLight=0xFFE42D,
    // If you are using no color correction, use this
    FairyLightNCC=0xFF9D2A

  } HTMLColorCode;
};

//-----------------------------------------------------------------------------
// From FastLED/src/lib8tion.h
//-----------------------------------------------------------------------------

typedef uint8_t fract8;
typedef int8_t sfract7;
typedef uint16_t  fract16;
typedef int16_t   sfract15;

typedef uint16_t  accum88;
typedef int16_t   saccum78;
typedef uint32_t  accum1616;
typedef int32_t   saccum1516;
typedef uint16_t  accum124;
typedef int32_t   saccum114;

extern "C" {
inline void * memmove8(void * dst, const void * src, uint16_t num) {
  (void) dst;
  (void) src;
  (void) num;
  return nullptr;
}

inline void * memcpy8(void * dst, const void * src, uint16_t num) {
  (void) dst;
  (void) src;
  (void) num;
  return nullptr;
}

inline void * memset8(void * ptr, uint8_t value, uint16_t num) {
  (void) ptr;
  (void) value;
  (void) num;
  return nullptr;
}
}

inline uint8_t random8() { return 0; }

inline uint8_t random8(uint8_t lim) {
  (void) lim;
  return 0;
}

inline uint8_t random8(uint8_t min, uint8_t lim) {
  (void) min;
  (void) lim;
  return 0;
}

inline uint16_t random16() { return 0; }

inline uint16_t random16( uint16_t lim) {
  (void) lim;
  return 0;
}

inline uint16_t random16( uint16_t min, uint16_t lim) {
  (void) min;
  (void) lim;
  return 0;
}

inline uint8_t beatsin8(
    accum88 beats_per_minute,
    uint8_t lowest = 0,
    uint8_t highest = 255,
    uint32_t timebase = 0,
    uint8_t phase_offset = 0) {
  (void) beats_per_minute;
  (void) lowest;
  (void) highest;
  (void) timebase;
  (void) phase_offset;
  return 0;
}

inline uint16_t beatsin16(
    accum88 beats_per_minute,
    uint16_t lowest = 0,
    uint16_t highest = 65535,
    uint32_t timebase = 0,
    uint16_t phase_offset = 0) {
  (void) beats_per_minute;
  (void) lowest;
  (void) highest;
  (void) timebase;
  (void) phase_offset;
  return 0;
}

inline uint16_t beat16(accum88 beats_per_minute, uint32_t timebase = 0) {
  (void) beats_per_minute;
  (void) timebase;
  return 0;
}

#define EVERY_N_MILLISECONDS(N) while (false)

//-----------------------------------------------------------------------------
// FastLED/src/lib8tion/trig8.h,
//-----------------------------------------------------------------------------

inline uint8_t sin8(uint8_t theta) {
  (void) theta;
  return 0;
}

inline uint8_t cos8(uint8_t theta) {
  (void) theta;
  return 0;
}

//-----------------------------------------------------------------------------
// FastLED/src/lib8tion/scale8.h
//-----------------------------------------------------------------------------

inline uint8_t scale8(uint8_t i, fract8 scale) {
  return ((uint16_t) i * (uint16_t) scale) >> 8;
}

inline uint16_t scale16( uint16_t i, fract16 scale) {
  return ((uint32_t) i * (uint32_t) scale) / 65536;
}

//-----------------------------------------------------------------------------
// From FastLED/src/colorutils.h
//-----------------------------------------------------------------------------

typedef uint32_t TProgmemRGBPalette16[16];

class CRGBPalette16 {
  public:
    CRGB entries[16];

    CRGBPalette16(const TProgmemRGBPalette16& rhs) {
      for(uint8_t i = 0; i < 16; ++i) {
        entries[i] =  rhs[i];
      }
    }
};

typedef enum { NOBLEND=0, LINEARBLEND=1 } TBlendType;

inline CRGB ColorFromPalette(
    const CRGBPalette16& pal,
    uint8_t index,
    uint8_t brightness=255,
    TBlendType blendType=LINEARBLEND) {
  (void) pal;
  (void) index;
  (void) brightness;
  (void) blendType;
  return CRGB();
}

inline void fadeToBlackBy(CRGB* leds, uint16_t num_leds, uint8_t fadeBy) {
  (void) leds;
  (void) num_leds;
  (void) fadeBy;
}

inline void fill_solid(
    struct CRGB * leds,
    int numToFill,
    const struct CRGB& color) {
  (void) leds;
  (void) numToFill;
  (void) color;
}

//-----------------------------------------------------------------------------
// From FastLED/src/colorpalettes.h
//-----------------------------------------------------------------------------

extern const TProgmemRGBPalette16 PartyColors_p;

//-----------------------------------------------------------------------------
// From FastLED/src/controller.h
//-----------------------------------------------------------------------------

#define DISABLE_DITHER 0x00
#define BINARY_DITHER 0x01

//-----------------------------------------------------------------------------
// From FastLED/src/FastLED.h
//-----------------------------------------------------------------------------

class CFastLED {
  // int m_nControllers;
  uint8_t  m_Scale;         ///< The current global brightness scale setting
  uint16_t m_nFPS;          ///< Tracking for current FPS value

public:
  CFastLED() {}

  /// Set the global brightness scaling
  /// @param scale a 0-255 value for how much to scale all leds before writing them out
  void setBrightness(uint8_t scale) { m_Scale = scale; }

  /// Get the current global brightness setting
  /// @returns the current global brightness value
  uint8_t getBrightness() { return m_Scale; }

  /// Set the maximum power to be used, given in volts and milliamps.
  /// @param volts - how many volts the leds are being driven at (usually 5)
  /// @param milliamps - the maximum milliamps of power draw you want
  void setMaxPowerInVoltsAndMilliamps(uint8_t volts, uint32_t milliamps) {
    (void) volts;
    (void) milliamps;
  }

  /// Set the maximum power to be used, given in milliwatts
  /// @param milliwatts - the max power draw desired, in milliwatts
  void setMaxPowerInMilliWatts(uint32_t milliwatts) {
    (void) milliwatts;
  }

  /// Update all our controllers with the current led colors, using the passed in brightness
  /// @param scale temporarily override the scale
  void show(uint8_t scale) {
    (void) scale;
  }

  /// Update all our controllers with the current led colors
  void show() { show(m_Scale); }

  /// clear the leds, wiping the local array of data, optionally black out the leds as well
  /// @param writeData whether or not to write out to the leds as well
  void clear(bool writeData = false) {
    (void) writeData;
  }

  /// clear out the local data array
  void clearData() {}

  /// Set all leds on all controllers to the given color/scale
  /// @param color what color to set the leds to
  /// @param scale what brightness scale to show at
  void showColor(const struct CRGB & color, uint8_t scale) {
    (void) color;
    (void) scale;
  }

  /// Set all leds on all controllers to the given color
  /// @param color what color to set the leds to
  void showColor(const struct CRGB & color) { showColor(color, m_Scale); }

  /// Delay for the given number of milliseconds.  Provided to allow the library to be used on platforms
  /// that don't have a delay function (to allow code to be more portable).  Note: this will call show
   /// constantly to drive the dithering engine (and will call show at least once).
  /// @param ms the number of milliseconds to pause for
  void delay(unsigned long ms) {
    (void) ms;
  }

  /// Set a global color temperature.  Sets the color temperature for all added led strips, overriding whatever
  /// previous color temperature those controllers may have had
  /// @param temp A CRGB structure describing the color temperature
  void setTemperature(const struct CRGB & temp) {
    (void) temp;
  }

  /// Set a global color correction.  Sets the color correction for all added led strips,
  /// overriding whatever previous color correction those controllers may have had.
  /// @param correction A CRGB structure describin the color correction.
  void setCorrection(const struct CRGB & correction) {
    (void) correction;
  }

  /// Set the dithering mode.  Sets the dithering mode for all added led strips, overriding
  /// whatever previous dithering option those controllers may have had.
  /// @param ditherMode - what type of dithering to use, either BINARY_DITHER or DISABLE_DITHER
  void setDither(uint8_t ditherMode = BINARY_DITHER) {
    (void) ditherMode;
  }

  /// Set the maximum refresh rate.  This is global for all leds.  Attempts to
  /// call show faster than this rate will simply wait.  Note that the refresh rate
  /// defaults to the slowest refresh rate of all the leds added through addLeds.  If
  /// you wish to set/override this rate, be sure to call setMaxRefreshRate _after_
  /// adding all of your leds.
  /// @param refresh - maximum refresh rate in hz
  /// @param constrain - constrain refresh rate to the slowest speed yet set
  void setMaxRefreshRate(uint16_t refresh, bool constrain=false) {
    (void) refresh;
    (void) constrain;
  }

  /// for debugging, will keep track of time between calls to countFPS, and every
  /// nFrames calls, it will update an internal counter for the current FPS.
  /// @todo make this a rolling counter
  /// @param nFrames - how many frames to time for determining FPS
  void countFPS(int nFrames=25) {
    (void) nFrames;
  }

  /// Get the number of frames/second being written out
  /// @returns the most recently computed FPS value
  uint16_t getFPS() { return m_nFPS; }

  /// Get how many controllers have been registered
  /// @returns the number of controllers (strips) that have been added with addLeds
  int count() { return 0; }

  /// Get the number of leds in the first controller
  /// @returns the number of LEDs in the first controller
  int size() { return 0; }

  /// Get a pointer to led data for the first controller
  /// @returns pointer to the CRGB buffer for the first controller
  CRGB *leds() { return nullptr; }
};

extern CFastLED FastLED;

//-----------------------------------------------------------------------------
// From FastLED/src/power_mgt.h
//-----------------------------------------------------------------------------

inline void set_max_power_in_volts_and_milliamps(
    uint8_t volts, uint32_t milliamps) {
  (void) volts;
  (void) milliamps;
}

#endif
