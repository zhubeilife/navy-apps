#include <NDL.h>
#include <SDL.h>
#include <stdio.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  char key_e_type[8];
  char key_num[16];
  if (NDL_PollEvent(buf, sizeof(buf))) {
    printf("now receive event: %s\n", buf);
    sscanf(buf, "%s %s\n", key_e_type, key_num);
    if (strcmp("ku", key_e_type) == 0) {
      ev->type = SDL_KEYUP;
      ev->key.type = SDL_KEYUP;
    }
    else if (strcmp("kd", key_e_type) == 0) {
      ev->type = SDL_KEYDOWN;
      ev->key.type = SDL_KEYDOWN;
    }
    else {
      printf("unrecognized key type: %s\n", key_e_type);
      return 0;
    }

    for (int i = 0; i < sizeof(keyname); i++) {
      if (strcmp(key_num, keyname[i]) == 0) {
        ev->key.keysym.sym = i;
        return 1;
      }
    }

    printf("unrecognized key num: %s\n", key_num);
    return 0;
  }
  return 0;
}

// Wait indefinitely for the next available event.
// (int) Returns 1 on success or 0 if there was an error while waiting for events;
int SDL_WaitEvent(SDL_Event *event) {
  while (1) {
    int poll_ret = SDL_PollEvent(event);
    if (poll_ret == 1) return 1;
  }
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
