#include <stddef.h>
#include <stdint.h>

/* Portable headers */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;

struct vga {
  size_t term_row;
  size_t term_column;
  uint8_t term_color;
  volatile uint16_t *term_buf;
};
vga vgaobj;

/* Hardware text mode color constants */
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define TEXT_COLOR_LIGHT_GREY 7
#define TEXT_COLOR_DARK_GREY 8
#define TEXT_COLOR_LIGHT_BLUE 9
#define TEXT_COLOR_LIGHT_GREEN 10
#define TEXT_COLOR_LIGHT_CYAN 11
#define TEXT_COLOR_LIGHT_RED 12
#define TEXT_COLOR_LIGHT_MAGENTA 13
#define TEXT_COLOR_LIGHT_BROWN 14
#define TEXT_COLOR_WHITE 15

uint8_t make_color(uint8_t fg, uint8_t bg) {
  return fg | bg << 4;
}

uint16_t vga_entry(uint8_t c, uint8_t color) {
  return (uint16_t) c | (uint16_t) color << 8;
}

size_t strlen(const char *str) {
  size_t ret = 0;
  while (str[ret] != 0) ret++;
  return ret;
}

void term_init() {
  vgaobj.term_row = 0;
  vgaobj.term_column = 0;
  vgaobj.term_color = make_color(TEXT_COLOR_WHITE, COLOR_BLUE);
  vgaobj.term_buf = (volatile uint16_t *)0xB8000;
  size_t index = 0;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      vgaobj.term_buf[index++] = vga_entry(' ', vgaobj.term_color);
    }
  }
}
/* In case the user wants to change color */
inline void term_setcolor(uint8_t color) { vgaobj.term_color = color; }

void term_putentry(uint8_t c, uint8_t color, size_t x, size_t y) {
  vgaobj.term_buf[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void term_putchar(char c) {
  term_putentry(c, vgaobj.term_color, vgaobj.term_column, vgaobj.term_row);
  if (++vgaobj.term_column == VGA_WIDTH) {
    vgaobj.term_column = 0;
    if (++vgaobj.term_row == VGA_HEIGHT) {
      vgaobj.term_row = 0;
    }
  }
}

void term_writestr(const char *data) {
  size_t datalen = strlen(data);
  for (size_t i = 0; i < datalen; i++) {
    char c = data[i];
    term_putentry(c, vgaobj.term_color, vgaobj.term_column, vgaobj.term_row);
    if (++vgaobj.term_column == VGA_WIDTH) {
      vgaobj.term_column = 0;
      if (++vgaobj.term_row == VGA_HEIGHT) {
        vgaobj.term_row = 0;
      }
    }
  }
}

extern "C" void _kernel_main() {
  term_init();
  term_writestr("Hello, world! >>Simple OS<<");
  while (1) {}
}
