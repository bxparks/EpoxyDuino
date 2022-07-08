/*
 * Test reading and writing from stdin and stdout, using the keyboard or pipes.
 *
 * Usage:
 *
 * To test keyboard input:
 *    $ ./StdioSerialEcho.out
 *    Echo test
 *    'char' is signed.
 *    # Type 'a', 'b', 'c, 'd' on keyboad
 *    61('a')62('b')63('c')64('d')
 *
 * To test reading of 0xFF character (which should not interfere with -1 error):
 *    $ printf '\xff' | ./StdioSerialEcho.out
 *    Echo test
 *    'char' is signed.
 *    FF(' ')
 *
 * To test reading from a directory, which generates a -1 error status when
 * ::read() is called:
 *    $ ./StdioSerialEcho.out < .
 *    Echo test
 *    'char' is signed.
 *    # Nothing should print.
 *
 * To test piping:
 *    $ yes | ./StdioSerialEcho.out
 *    Echo test
 *    'char' is signed.
 *    79('y')0A(' ')79('y')0A(' ')[...]
 */

#include <Arduino.h>

/**
 * Print the hexadecimal value of the character. If the character is printable
 * in ASCII, print its character inside parenthesis. Print a space for
 * non-printable character. Example, "61('a')" if the 'a' is passed as the
 * argument. This is useful for determining if an 0xFF byte can be read from the
 * Serial port without interfering with the -1 error indicator.
 */
void debugPrint(int c) {
  if (c < 0) {
    // This should never happen, but print the value of c if it does.
    Serial.print(c);
  } else {
    // Print 2 digit hex padded with 0.
    if (c < 16) {
      Serial.print('0');
    }
    Serial.print(c, 16);
  }

  // Print printable character, or space for non-printable.
  Serial.print("('");
  Serial.print((c > 32 && c < 127) ? (char) c : ' ');
  Serial.print("')");
}

/** Read the Serial port using an explicit while-loop. */
void loopExplicitly() {
  while (true) {
    if (Serial.available()) {
      int c = Serial.read();
      debugPrint(c);
    }
  }
}

/**
 * Read the Serial port using the implicit Arduino loop(), with a non-blocking
 * one second delay to test the buffering of the keyboard input.
 */
void loopImplicitly() {
  static uint16_t prevMillis = 0;

  uint16_t nowMillis = millis();
  if ((uint16_t) (nowMillis - prevMillis) > 1000) {
    prevMillis = nowMillis;
    if (Serial.available()) {
      int c = Serial.read();
      debugPrint(c);
    }
  }
}

//-----------------------------------------------------------------------------

void setup(void) {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif

  Serial.begin(115200);
  while (!Serial);

#if defined(EPOXY_DUINO)
  Serial.setLineModeUnix();
#endif

  // Check if 'char' is a signed or unsigned on this system.
  Serial.println(F("Echo test"));
  char c = (char) 128;
  int i = c;
  if (i < 0) {
    Serial.println(F("'char' is signed."));
  } else {
    Serial.println(F("'char' is unsigned."));
  }
}

void loop(void) {
  // Choose one of the following loop methods to test Serial.available()
  // and Serial.read(). Both of them should work:
  loopExplicitly();
  //loopImplicitly();
}
