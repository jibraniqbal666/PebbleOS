#include "drivers/mcu.h"

#define CMSIS_COMPATIBLE
#include <mcu.h>

StatusCode mcu_get_serial(void *buf, size_t *buf_sz) {
  return E_DOES_NOT_EXIST;
}

uint32_t mcu_cycles_to_milliseconds(uint64_t cpu_ticks) {
#if defined(MICRO_FAMILY_ESP32C3)
  // ESP32-C3 default CPU frequency is 160MHz (from sdkconfig.h)
  const uint32_t ESP32C3_CPU_FREQ_HZ = 160000000UL;
  return ((cpu_ticks * 1000) / ESP32C3_CPU_FREQ_HZ);
#else
  return ((cpu_ticks * 1000) / SystemCoreClock);
#endif
}
