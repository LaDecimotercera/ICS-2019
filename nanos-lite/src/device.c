#include "common.h"
#include <amdev.h>
#include "proc.h"

size_t serial_write(const void *buf, size_t offset, size_t len) {
  //_yield();
  for (size_t i = 0; i < len; ++i)
    _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

#define KEYDOWN_MASK 0x8000

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  //Use the api of IOE to get input
  //_yield();
  int keycode = read_key();
  if ((keycode & ~KEYDOWN_MASK) == _KEY_NONE) 
    sprintf(buf, "t %d\n", uptime());
  else if ((keycode & KEYDOWN_MASK) != _KEY_NONE) {
    int key_code = keycode & ~KEYDOWN_MASK;
    sprintf(buf, "kd %s\n", keyname[key_code]);
    switch (key_code)
    {
    case _KEY_F1: Log("F1 PRESSED"); fg_pcb = 1; break;
    case _KEY_F2: Log("F2 PRESSED"); fg_pcb = 2; break;
    case _KEY_F3: Log("F3 PRESSED"); fg_pcb = 3; break;
    default: break;
    }
  } else {
    int key_code = keycode & ~KEYDOWN_MASK;
    sprintf(buf, "ku %s\n", keyname[key_code]);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //fix
  if (offset + len > 128) len = 128 - offset;
  memcpy(buf, (void *)dispinfo + offset, len);
  ((char*)buf)[len] = '\0';
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //_yield();
  int x = (offset / sizeof(uint32_t)) % screen_width();
  int y = (offset / sizeof(uint32_t)) / screen_width();
  int w = len / sizeof(uint32_t);
  int h = 1; 
  draw_rect((uint32_t *)buf, x, y, w, h);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height()); 
}
