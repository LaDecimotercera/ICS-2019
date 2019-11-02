#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000
#define I8042_DATA_PORT 0x60

//unsigned long long kbs;
size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {//modified
    // _DEVREG_INPUT_KBD, AM键盘控制器.
    // 从中读出_DEV_INPUT_KBD_t结构体, 
    // keydown = 1为按下按键, keydown = 0为释放按键. keycode为按键的断码, 没有按键时, keycode为_KEY_NONE.
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      kbd->keycode = _KEY_NONE;
      if (inb(0x64)&0x1)
        kbd->keycode=inl(I8042_DATA_PORT);
      kbd->keydown = 0;
      //kbs = inl(I8042_DATA_PORT);
      //kbd->keycode = kbs;
      //printf("???: %d\n",kbd->keycode);
      //if (kbd->keycode != _KEY_NONE)
      //  kbd->keydown = !(kbd->keydown);
      //kbd->keydown=0;
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
