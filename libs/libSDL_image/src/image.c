#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include <stdio.h>

#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

/*
1- 用libc中的文件操作打开文件, 并获取文件大小size
2- 申请一段大小为size的内存区间buf
3- 将整个文件读取到buf中
4- 将buf和size作为参数, 调用STBIMG_LoadFromMemory(), 它会返回一个SDL_Surface结构的指针
5- 关闭文件, 释放申请的内存
6- 返回SDL_Surface结构指针
 */
SDL_Surface* IMG_Load(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    printf("could not open file %s\n", filename);
    return NULL;
  }

  fseek(fp, 0, SEEK_END); // Seek to the end of the file
  long size = ftell(fp); // Get the current position (which is the file size)
  unsigned char *buf = (unsigned char *)malloc(size);
  fseek(fp, 0, SEEK_SET);
  fread(buf, 1, size, fp);

  SDL_Surface* img = STBIMG_LoadFromMemory(buf, size);
  fclose(fp);
  free(buf);
  return img;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
