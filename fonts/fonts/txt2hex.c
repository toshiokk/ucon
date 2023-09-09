// gcc -o txt2hex txt2hex.c 2>_error

#include <stdio.h>
#include <ctype.h>
#include <string.h>

int fnt2hex();

int main(int argc, char *argv[])
{
	if (argc > 1) {
		if (strcmp(argv[1], "hex2txt") == 0)
			;
		else if (strcmp(argv[1], "txt2hex") == 0)
			fnt2hex();
	}
	return 0;
}

// 0021:!
// ........
// ........
// ........
// ........
// ....#...
// ....#...
// ....#...
// ....#...
// ....#...
// ....#...
// ....#...
// ........
// ....#...
// ....#...
// ........
// ........
int fnt2hex()
{
	char buffer[100+1];
	int line_idx;
	int len;
	unsigned char bit;
	unsigned char byte;
	int idx;
	int line_output = 0;

	line_idx = 0;
	while(fgets(buffer, 100, stdin)) {
		if (isxdigit(buffer[0])) {
			if (line_output) {
				line_output = 0;
				printf("\n");
			}
			buffer[5] = '\0';
			printf("%s", buffer);
			line_idx = 0;
		} else {
			line_output = 1;
			len = strlen(buffer);
			bit = 0x00;
			byte = 0;
			for (idx = 0; idx < len; idx++) {
				if (bit == 0x00)
					bit = 0x80;
				byte = byte | (buffer[idx] == '#' ? bit : 0x00);
				bit >>= 1;
				if ((idx % 8) == 7) {
					printf("%02X", byte);
					byte = 0;
				}
			}
			line_idx++;
		}
	}
	if (line_output) {
		printf("\n");
	}
}
