// copy from https://gitlab.eduxiji.net/educg-group-22022-2210132/T202410487992568-3560/-/blob/pa3/navy-apps/libs/libam/src/ioe.c
#include <am.h>
#include <stdio.h>
#include <string.h>
#include <NDL.h>

static void fail(void *buf) { printf("access nonexist register"); }

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }

static void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime){
  uptime->us = NDL_GetTicks() * 1000;
}

static void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 0;
}

#define _KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
  _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
  _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
  _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
  _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
  _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define keyname(k) #k,

static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};

static void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd){
  char buf[128];
  if (NDL_PollEvent(buf, 128) == 1)
  {
    //printf("SDL_PollEvent: %s", buf);
    if (strncmp(buf, "kd", 2) == 0)
    {
      kbd->keydown = true;
    }
    else
    {
      kbd->keydown = false;
    }
    for (unsigned i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i)
    {
      if (strncmp(buf + 3, keyname[i], strlen(buf) - 4) == 0 && buf[3 + strlen(keyname[i])] == '\n')
      {
        //printf("keyname[%d]: %s\n", i, keyname[i]);
        kbd->keycode = i;
        break;
      }
    }
  }
  else {
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  }
}

static int w = 0,h = 0;

static void __am_gpu_config(AM_GPU_CONFIG_T *cfg){
  cfg->width = w;
  cfg->height = h;
  cfg->vmemsz = cfg->width * cfg->height * 4;
  cfg->present = true;
  cfg->has_accel = false;
}

static void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl){
  NDL_DrawRect(ctl->pixels,ctl->x, ctl->y, ctl->w, ctl->h);
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
  [AM_INPUT_CONFIG] = __am_input_config,
  [AM_INPUT_KEYBRD] = __am_input_keybrd,
  [AM_GPU_CONFIG  ] = __am_gpu_config,
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,
  [AM_GPU_STATUS  ] = __am_gpu_status,
};

bool ioe_init() {
  NDL_Init(0);
  NDL_OpenCanvas(&w,&h);
  for (int i = 0; i < 128; i++)
    if (!lut[i]) lut[i] = fail;
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)(lut[reg]))(buf);}
void ioe_write(int reg, void *buf) { ((handler_t)(lut[reg]))(buf);}
