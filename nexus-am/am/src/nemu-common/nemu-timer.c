#include <am.h>
#include <amdev.h>
#include <nemu.h>

# define RTC_PORT     0x48

uint32_t cur_time;  
size_t __am_timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {//modified
      _DEV_TIMER_UPTIME_t *uptime = (_DEV_TIMER_UPTIME_t *)buf;
      //cur_time = (uptime->hi << 32LL) | uptime->lo;
      
      uptime->hi = 0; 
      uptime->lo = inl(RTC_PORT) - cur_time; 
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
  cur_time = inl(RTC_PORT);
}
