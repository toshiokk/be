// generate 8 bit ASCII character table

// 0020:  
// 0040:
// 0060:
// 0080:
// 00a0:
//     :
// 00e0:

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	for (int base = 0x00; base < 0x80; base += 0x20) {
		printf("%04x: ", base);
		for (int off = 0; off < 0x20; off++) {
			char chr = base + off;
			if (chr == 0x09 || chr == 0x0a || chr == 0x0d) {
				putchar(' ');
			} else {
				putchar(chr);
			}
		}
		printf("\n");
	}
	for (int base = 0x80; base < 0x100; base += 0x20) {
		printf("%04x: ", base);
		if (argc <= 1) {
			for (int off = 0; off < 0x20; off++) {
				char chr = base + off;
				if (chr == 0x09 || chr == 0x0a || chr == 0x0d) {
					putchar(' ');
				} else {
					putchar(chr);
				}
			}
		} else {
			for (int off = 0; off < 0x20; off++) {
				char chr = base + off;
				printf("%c%c", 0xc0 + ((chr & 0xc0) >> 6), 0x80 + (chr & 0x3f));
			}
		}
		printf("\n");
	}
}
