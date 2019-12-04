#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char ch[len];
  memcpy(ch, buf, len);
  for (size_t i = 0; i < len; i ++)
    _putc(ch[i]);
  return 0;
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
  int keycode = read_key();
  if ((keycode & ~KEYDOWN_MASK) == _KEY_NONE) 
    sprintf(buf, "t %d\n", uptime());
  else if ((keycode & KEYDOWN_MASK) != 0) {
    int key_code = keycode & ~KEYDOWN_MASK;
    sprintf(buf, "kd %s\n", keyname[key_code]);
  } else {
    int key_code = keycode & ~KEYDOWN_MASK;
    sprintf(buf, "ku %s\n", keyname[key_code]);
  }
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t get_dispinfo_size() {
  return strlen(dispinfo);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //fix
  //if (offset + len > 128) len = 128 - offset;
  //memcpy(buf, (void *)dispinfo + offset, len);
  //((char*)buf)[len] = '\0';
  //return len;
  strncpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  /*int x = (offset / sizeof(uint32_t)) % screen_width();
  int y = (offset / sizeof(uint32_t)) / screen_width();
  int w = len / sizeof(uint32_t);
  int h = 1; 
  draw_rect((uint32_t *)buf, x, y, w, h);
  return len;*/
  int x, y;
  assert(offset + len <= (size_t)screen_height() * screen_width() * 4);
  x = (offset / 4) % screen_width();
  y = (offset / 4) / screen_width();
  assert(x + len < (size_t)screen_width() * 4);
  draw_rect((void *)buf, x, y, len / 4, 1);
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
