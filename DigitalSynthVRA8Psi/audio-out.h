// refs http://playground.arduino.cc/Code/PCMAudio

#pragma once

#include "common.h"

template <uint8_t T>
class AudioOut {
  static const int SPEAKER_PIN = 3;   // PD3 (OC2B)
  static const int LED_PIN     = 13;  // PB5

  static uint8_t m_count;
  static uint8_t m_maxTcnt;
  static uint8_t m_busy;
  static uint8_t m_busyCont;

public:
  INLINE static void open() {
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(LED_PIN,     OUTPUT);

    // Timer/Counter2 (8-bit Fast PWM, Inverting, 125 kHz)
    TCCR2A = 0x33;
    TCCR2B = 0x09;
    OCR2A  = 0x7F;

    m_count = 0;
    m_maxTcnt = 0;
    m_busy = 0;
    m_busyCont = 0;
  }

  INLINE static void write(int8_t level) {
#if defined(DEBUG)
    // Output Elapsed Time as Channel Pressure (of Channel 16)
    m_count++;
    if (m_count == 0x7F) {
      UDR0 = 0xDF;
    } else if (m_count == 0xFF) {
#if 0
      uint8_t tcnt = TCNT2 >> 2;
      if ((tcnt < 64) && (tcnt > m_maxTcnt)) {
        m_maxTcnt = tcnt;
      }
      tcnt = m_maxTcnt;
#elif 1
      uint8_t tcnt = m_busyCont;
      tcnt &= 0x7F;
#elif 1
      uint8_t tcnt = TCNT2 >> 2;
      if (tcnt >= 64) {
        tcnt = 0;   // Not Over
      }
#endif
      UDR0 = tcnt;
      m_count = 0;
    }
#endif
    if (TIFR2 & _BV(TOV2)) {
      // CPU BUSY
      PORTB = _BV(5);
#if defined(DEBUG)
      if (m_busy) {
        m_busyCont++;
      }
      m_busy = 1;
#endif
    } else {
      PORTB = 0x00;
#if defined(DEBUG)
      m_busy = 0;
#endif
      while ((TIFR2 & _BV(TOV2)) == 0);
    }
    TIFR2 = _BV(TOV2);
    OCR2B = 0x3F - (level >> 1);
  }
};

template <uint8_t T> uint8_t AudioOut<T>::m_count;
template <uint8_t T> uint8_t AudioOut<T>::m_maxTcnt;
template <uint8_t T> uint8_t AudioOut<T>::m_busy;
template <uint8_t T> uint8_t AudioOut<T>::m_busyCont;
