#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define RTC_PORT 0x48   // Note that this is not the standard
#define RTC_MMIO 0xa1000048   //Copy from nemu/../timer.c

unsigned long long cur_time;  
size_t __am_timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {//modified
      _DEV_TIMER_UPTIME_t *uptime = (_DEV_TIMER_UPTIME_t *)buf;
      //(uptime->hi << 32LL) | uptime->lo;
      cur_time = inl(RTC_PORT);
      uptime->hi = cur_time >> 32; 
      uptime->lo = cur_time & 0xffffffff; 
      return sizeof(_DEV_TIMER_UPTIME_t);
    }
    case _DEVREG_TIMER_DATE: {
      _DEV_TIMER_DATE_t *rtc = (_DEV_TIMER_DATE_t *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2000;
      return sizeof(_DEV_TIMER_DATE_t);
    }
  }
  return 0;
}

void __am_timer_init() {
}
