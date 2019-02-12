// refs http://playground.arduino.cc/Code/PCMAudio

#pragma once

#include "common.h"

template <uint8_t T>
class AudioOut {
  static const int SPEAKER_PIN = 5;   // PD5 (OC0B)
  static const int LED_PIN     = 13;  // PB5

  static uint8_t m_count;

public:
  INLINE static void open() {
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(LED_PIN,     OUTPUT);

    // Timer/Counter0 (8-bit Fast PWM)
    TCCR0A = 0x23;
    TCCR0B = 0x09;
    OCR0A  = 0x7f;

    m_count = 0;
  }

  INLINE static void write(int8_t level) {
#if defined(DEBUG)
    // Output Elapsed Time as Channel Pressure
    m_count++;
    if (m_count == 0x7F) {
      UDR0 = 0xDF;
    } else if (m_count == 0xFF) {
      uint8_t cnt = TCNT0;
#if 0
      static uint8_t s_maxCnt = 0;
      if ((cnt < 64) && (cnt > s_maxCnt)) {
        s_maxCnt = cnt;
      }
      cnt = s_maxCnt;
#elif 1
      if (cnt >= 64) {
        cnt = 99;   // Not Over
      }
#endif
      UDR0 = cnt;
      m_count = 0;
    }
#endif
    if (TIFR0 & _BV(TOV0)) {
      // CPU BUSY
      PORTB |= _BV(5);
    } else {
      PORTB &= ~_BV(5);
    }
    while ((TIFR0 & _BV(TOV0)) == 0);
    TIFR0 = _BV(TOV0);
    OCR0B = 0x40 - (level >> 1);
  }
};

template <uint8_t T> uint8_t AudioOut<T>::m_count;
