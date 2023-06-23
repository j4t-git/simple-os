#include <stddef.h>
#include <stdint.h>
/* Portable headers */

struct vga {
  static const size_t VGA_WIDTH = 80;
  static const size_t VGA_HEIGHT = 24;
  size_t term_row;
  size_t term_column;
  uint8_t term_color;
  volatile uint16_t *term_buf;
};
vga vgaobj;

/* Hardware text mode color constants */

enum vga_color {
  COLOR_BLACK,
  COLOR_BLUE,
  COLOR_GREEN,
  COLOR_CYAN,
  COLOR_RED,
  COLOR_MAGENTA,
  COLOR_BROWN,
  TEXT_COLOR_LIGHT_GREY,
  TEXT_COLOR_DARK_GREY,
  TEXT_COLOR_LIGHT_BLUE,
  TEXT_COLOR_LIGHT_GREEN,
  TEXT_COLOR_LIGHT_CYAN,
  TEXT_COLOR_LIGHT_RED,
  TEXT_COLOR_LIGHT_MAGENTA,
  TEXT_COLOR_LIGHT_BROWN,
  TEXT_COLOR_WHITE
};

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

uint16_t vga_entry(uint8_t c, uint8_t color) {
  uint16_t c16 = c;
  uint16_t color16 = color;
  return c16 | color16 << 8;
}

size_t strlen(const char *str) {
  size_t ret = 0;
  while (str[ret] != 0) {
    ret++;
  }
  return ret;
}

void term_init() {
  vgaobj.term_row = 0;
  vgaobj.term_column = 0;
  vgaobj.term_color = make_color(TEXT_COLOR_WHITE, COLOR_BLUE);
  vgaobj.term_buf = (volatile uint16_t *)0xB8000;
  for (size_t y = 0; y < vgaobj.VGA_HEIGHT; y++) {
    for (size_t x = 0; x < vgaobj.VGA_WIDTH; x++) {
      const size_t index = y * vgaobj.VGA_WIDTH + x;
      vgaobj.term_buf[index] = vga_entry(' ', vgaobj.term_color);
    }
  }
}
/* In case the user wants to change color */
inline void term_setcolor(uint8_t color) { vgaobj.term_color = color; }

void term_putentry(uint8_t c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * vgaobj.VGA_WIDTH + x;
  vgaobj.term_buf[index] = vga_entry(c, color);
}

void term_putchar(char c) {
  term_putentry(c, vgaobj.term_color, vgaobj.term_column, vgaobj.term_row);
  if (++vgaobj.term_column == vgaobj.VGA_WIDTH) {
    vgaobj.term_column = 0;
    if (++vgaobj.term_row == vgaobj.VGA_HEIGHT) {
      vgaobj.term_row = 0;
    }
  }
}

void term_writestr(const char *data) {
  size_t datalen = strlen(data);
  for (size_t i = 0; i < datalen; i++) {
    term_putchar(data[i]);
  }
}

extern "C" void _kernel_main() {
  term_init();
  term_writestr("Hello, world! >>Simple OS<<");
  while (1) {
  };
}
