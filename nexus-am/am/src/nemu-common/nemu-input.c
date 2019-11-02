#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000
#define I8042_DATA_PORT 0x60

size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {//modified
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      kbd->keycode = inl(I8042_DATA_PORT);
      //printf("???: %d\n",kbd->keycode);
      if (kbd->keycode != _KEY_NONE)
        kbd->keydown = !(kbd->keydown);
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
