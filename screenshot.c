/*
 * fbshot.c -- FrameBuffer Screen Capture Utility
 * (C)opyright 2002 sfires@sfires.net
 *
 * Originally Written by: Stephan Beyer <PH-Linex@gmx.net>
 * Further changes by: Paul Mundt <lethal@chaoticdreams.org>
 * Rewriten and maintained by: Dariusz Swiderski <sfires@sfires.net>
 * 
 * 	This is a simple program that generates a
 * screenshot of the specified framebuffer device and
 * terminal and writes it to a specified file using
 * the PNG format.
 *
 * See ChangeLog for modifications, CREDITS for credits.
 * 
 * fbshot is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either Version 2
 * of the License, or (at your option) any later version.
 *
 * fbshot is distributed in the hope that it will be useful, but
 * WITHOUT ANY  WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License with fbshot; if not, please write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 
 * 02111-1307 USA
 */

#include <png.h>
#include "ucon.h"
#include "screenshot.h"

PRIVATE char *screen_shot_file_name(void);
PRIVATE int alloc_buffer(int size_x, int size_y);
PRIVATE int free_buffer(int size_x, int size_y);
PRIVATE int capture_fb(int size_x, int size_y,
 argb32_t (*get_pixel_argb32)(u_short xx, u_short yy));
PRIVATE int write_png(int size_x, int size_y, const char *filename);

int screen_shot(int size_x, int size_y,
 argb32_t (*get_pixel_argb32)(u_short xx, u_short yy), void (*reverse_all)(void),
 const char *file_name)
{
	if (file_name == NULL) {
		file_name = screen_shot_file_name();
	}
	_FLF0_
	if (alloc_buffer(size_x, size_y)) {
		_FLF_
		return -1;
	}
	if (capture_fb(size_x, size_y, get_pixel_argb32)) {
		_FLF_
		return -2;
	}
_MFLF_
	reverse_all();		// flash screen
	reverse_all();		// recover screen
_MFLF_
flf_d_printf("[%s]\n", file_name);
	if (write_png(size_x, size_y, file_name)) {
		_FLF_
		return -3;
	}
	free_buffer(size_x, size_y);
	_FLF9_
	return 0;
}
PRIVATE char *screen_shot_file_name(void)
{
#define MAX_PATH_LEN	256
	static char file_name[MAX_PATH_LEN+1];	// "/home/user/yymmdd_hhmmss.png"
	char yymmdd_hhmmss[13+1];		// "yymmdd_hhmmss"

	snprintf(file_name, MAX_PATH_LEN+1, "%s/%s.png",
	 getenv("HOME"), get_yymmdd_hhmmss(time(NULL), yymmdd_hhmmss));
	return file_name;
}

png_bytep *row_pointers = NULL;

PRIVATE int alloc_buffer(int size_x, int size_y)
{
	int yy;

	row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * size_y);
	if (row_pointers == NULL) {
		_FLF_
		return -1;
	}
	for (yy = 0; yy < size_y; yy++) {
		row_pointers[yy] = NULL;
	}
	for (yy = 0; yy < size_y; yy++) {
		row_pointers[yy] = (png_bytep)malloc(sizeof(u_int32) * size_x);
		if (row_pointers[yy] == NULL) {
			break;
		}
	}
	if (yy < size_y) {
		free_buffer(size_x, size_y);
		_FLF_
		return -2;
	}
	return 0;
}
PRIVATE int free_buffer(int size_x, int size_y)
{
	int yy;

	for (yy = 0; yy < size_y; yy++) {
		if (row_pointers[yy]) {
			free(row_pointers[yy]);
			row_pointers[yy] = NULL;
		}
	}
	free(row_pointers);
	return 0;
}
PRIVATE int capture_fb(int size_x, int size_y,
 argb32_t (*get_pixel_argb32)(u_short xx, u_short yy))
{
	int yy, xx;
	u_char *buf;
	argb32_t argb32;

	for (yy = 0; yy < size_y; yy++) {
		buf = (u_char *)row_pointers[yy];
		for (xx = 0; xx < size_x; xx++) {
			argb32 = get_pixel_argb32(xx, yy);
			*buf++ = argb32 >> 16;	// red
			*buf++ = argb32 >> 8;	// green
			*buf++ = argb32;		// blue
			*buf++ = argb32 >> 24;	// alpha
		}
	}
	return 0;
}

PRIVATE int write_png(int size_x, int size_y, const char *filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_text txt_ptr[4];
	int bit_depth, color_type, interlace;
	FILE *fp;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 
	 (png_voidp)NULL, (png_error_ptr)NULL, (png_error_ptr)NULL);
	if (!png_ptr) {
		_FLF_
		return -1;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		_FLF_
		return -2;
	}

	txt_ptr[0].key = (char *)"Name";
	txt_ptr[0].text = (char *)"Screenshot";
	txt_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
	txt_ptr[1].key = (char *)"Date";
	txt_ptr[1].text = (char *)"Current Date";
	txt_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
	txt_ptr[2].key = (char *)"Hostname";
	txt_ptr[2].text = (char *)"Host name";
	txt_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
	txt_ptr[3].key = (char *)"Program";
	txt_ptr[3].text = (char *)PACKAGE " Ver." VERSION;
	txt_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;
	png_set_text(png_ptr, info_ptr, txt_ptr, 4);

	fp = fopen(filename, "wb");
	if (!fp) {
		_FLF_
		return -3;
	}
	png_init_io(png_ptr, fp);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
	png_set_invert_alpha(png_ptr);

	bit_depth = 8;
	color_type = PNG_COLOR_TYPE_RGB_ALPHA;
	interlace = PNG_INTERLACE_NONE;
	png_set_IHDR(png_ptr, info_ptr, size_x, size_y,
	 bit_depth, color_type, interlace,
	 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return 0;
}

// End of screenshot.c
