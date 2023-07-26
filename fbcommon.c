/*
 * UCON -
 * Copyright (C) 1999 Noritoshi Masuichi (nmasu@ma3.justnet.ne.jp)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NORITOSHI MASUICHI ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NORITOSHI MASUICHI BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "ucon.h"

frame_buffer_t frame_buffer__;

PRIVATE int fb_select_driver(frame_buffer_t *fb,
 struct fb_var_screeninfo *fbvs,
 struct fb_fix_screeninfo *fbfs);
#ifdef ENABLE_DEBUG
PRIVATE void fb_show_screeninfo(frame_buffer_t *fb,
 struct fb_var_screeninfo *fbvs, struct fb_fix_screeninfo *fbfs);
#endif // ENABLE_DEBUG

PRIVATE void fb_setup_15_16_bpp_color_table(void);

PRIVATE int fb_get_var_screen_info(int fd_fb, struct fb_var_screeninfo *var);
PRIVATE int fb_set_var_screen_info(int fd_fb, struct fb_var_screeninfo *var);
PRIVATE int fb_get_fix_screen_info(int fd_fb, struct fb_fix_screeninfo *fix);
PRIVATE int fb_get_cmap(int fd_fb, struct fb_cmap *cmap);
PRIVATE int fb_put_cmap(int fd_fb, struct fb_cmap *cmap);
PRIVATE int fb_pan_display(int fd_fb, struct fb_var_screeninfo *var);

#ifndef major	/* defined in sys/sysmacros.h - ukai 1999/10/27 */
#define major(dev)	(((dev) >> 8) & 0xff)
#endif

PRIVATE u_short save_red[256], save_green[256], save_blue[256],
 save_trans[256];
PRIVATE struct fb_cmap save_color_map
 = { 0, 256, save_red, save_green, save_blue, save_trans };
PRIVATE bool_t color_map_saved = FALSE;

PRIVATE struct fb_var_screeninfo save_fb_var;
PRIVATE bool_t var_screen_info_modified = FALSE;

/*---------------------------------------------------------------------------*/
// 16 colors pallete
PRIVATE argb32_t table_16_color_argb32[COLORS_16] = {	// 00000000rrrrrrrrggggggggbbbbbbbb
//    AARRGGBB
	0x00000000,	//  0: black
	0x00a00000,	//  1: red
	0x0000a000,	//  2: green
	0x00606000,	//  3: brown
	0x000000a0,	//  4: blue
	0x00a000a0,	//  5: magenta
	0x0000a0a0,	//  6: cyan
	0x00a0a0a0,	//  7: lightgray
	0x00606060,	//  8: darkgray
	0x00ff0000,	//  9: lightred
	0x0000ff00,	// 10: lightgreen
	0x00c0c000,	// 11: yellow
	0x000000ff,	// 12: lightblue
	0x00ff00ff,	// 13: magenta
	0x0000ffff,	// 14: lightcyan
	0x00ffffff,	// 15: white
};
PRIVATE rgb16_t table_16_color_rgb16[COLORS_16];	// rrrrrggggggbbbbb
PRIVATE rgb15_t table_16_color_rgb15[COLORS_16];	// 0rrrrrgggggbbbbb

/*---------------------------------------------------------------------------*/

int fb_get_fb_dev_name(frame_buffer_t *fb)
{
	int fd;
	struct fb_con2fbmap fb_c2fm;
	struct vt_stat vt_st;
	char *env_fb_dev_name;

	fb->fd_tty0 = -1;
	fb->fd_fb = -1;
	memset(fb->fb_dev_name, 0, FB_DEV_NAME_LEN+1);

	if ((fb->fd_tty0 = util_privilege_open("/dev/tty0", O_RDWR)) < 0) {
		_ERR_
		printf_strerror("open /dev/tty0");
		return -1;
	}

	if (ioctl(fb->fd_tty0, VT_GETSTATE, &vt_st) < 0) {
		_ERR_
		printf_strerror("ioctl VT_GETSTATE");
		return -1;
	}
	fb_c2fm.console = vt_st.v_active;
#ifdef ENABLE_DIMMER
	fb->my_vt_num = vt_st.v_active;
#endif

	if ((env_fb_dev_name = getenv("FRAMEBUFFER")) != NULL) {
		strncpy(fb->fb_dev_name, env_fb_dev_name, FB_DEV_NAME_LEN);
	} else {
		if ((fd = util_privilege_open("/dev/fb0", O_RDWR)) < 0) {
			_ERR_
			printf_strerror("open /dev/fb0");
			return -1;
		}
		if (ioctl(fd, FBIOGET_CON2FBMAP, &fb_c2fm) < 0) {
			_ERR_
			perror("ioctl FBIOGET_CON2FBMAP");
			fb_c2fm.framebuffer = 0;
		}
		close(fd);

		snprintf(fb->fb_dev_name, FB_DEV_NAME_LEN+1, "/dev/fb%d", fb_c2fm.framebuffer);
	}
flf_d_printf("FB device name: [%s]\n", fb->fb_dev_name);
	return 0;
}

int fb_open(frame_buffer_t *fb)
{
	struct stat st;
	struct fb_var_screeninfo fb_var;
	struct fb_fix_screeninfo fb_fix;
	int fb_type_idx;

	if ((fb->fd_fb = util_privilege_open(fb->fb_dev_name, O_RDWR)) == -1) {
		_ERR_
		printf_strerror("open %s", fb->fb_dev_name);
		return -1;
	}
	if (fstat(fb->fd_fb, &st) < 0) {
		_ERR_
		printf_strerror("fstat(%s)", fb->fb_dev_name);
		return -1;
	}
	if (!S_ISCHR(st.st_mode) || major(st.st_rdev) != 29) {	/* FB_MAJOR */
		_ERR_
		printf_stderr("%s: not a framebuffer device\n", fb->fb_dev_name);
		return -1;
	}

	if (fb_get_var_screen_info(fb->fd_fb, &fb_var) < 0) {
		_ERR_
		return -1;
	}
	if (fb_var.yres_virtual != fb_var.yres) {
		memcpy_ol((void*)&save_fb_var, (void*)&fb_var, sizeof(save_fb_var));
		fb_var.yres_virtual = fb_var.yres;
		fb_var.yoffset = 0;
		fb_var.activate = FB_ACTIVATE_NOW;
		var_screen_info_modified = TRUE;
		if (fb_set_var_screen_info(fb->fd_fb, &fb_var) < 0) {
			_ERR_
			return -1;
		}
	}
	fb_get_fix_screen_info(fb->fd_fb, &fb_fix);

	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR
	 || fb_fix.visual == FB_VISUAL_PSEUDOCOLOR) {
		fb_get_cmap(fb->fd_fb, &save_color_map);
		color_map_saved = TRUE;
	}

#ifdef ENABLE_DEBUG
	if (app__.debug) {
		fb_show_screeninfo(fb, &fb_var, &fb_fix);
	}
#endif // ENABLE_DEBUG
	fb_type_idx = fb_select_driver(fb, &fb_var, &fb_fix);
	if (fb_type_idx < 0) {
		_ERR_
		printf_stderr("Oops: Unknown framebuffer ???\n");
		return -1;
	}
flf_d_printf("Selected driver index: %d, bits per pixel: %d\n",
 fb_type_idx, fb_drivers[fb_type_idx].bitsPerPixel);
	fb->driver = &fb_drivers[fb_type_idx];

	/* FIX: scanline length is not necessarily the same as display width */
	fb->width = fb_var.xres;
	fb->height = fb_var.yres;
	fb->bytes_per_line = fb_fix.line_length;
	fb->bits_per_pixel = fb_var.bits_per_pixel;
	fb->bytes_per_pixel = (fb_var.bits_per_pixel + (8-1)) / 8;
	verbose_printf("width:%d, height:%d, bits/pixel:%d, bytes/pixel:%d, bytes/line:%d\n",
	 fb->width, fb->height, fb->bits_per_pixel, fb->bytes_per_pixel, fb->bytes_per_line);

#ifndef PAGE_MASK
// PAGE_MASK is normally defined in <asm/page.h>
#define PAGE_MASK		(~(0x1000-1))	// 0xfffff000 : 11111111-11111111-11110000-00000000
#endif
	// mmap Buffer Memory
	fb->fb_offset = (u_int32)(fb_fix.smem_start) & (~PAGE_MASK);
	fb->fb_mem_size = (fb_fix.smem_len + fb->fb_offset + ~PAGE_MASK) & PAGE_MASK;
	fb->fb_view_size = fb->bytes_per_line * fb->height;
	fb->fb_mmapped = (u_char *)mmap(NULL, fb->fb_mem_size, PROT_READ|PROT_WRITE,
	 MAP_SHARED, fb->fd_fb, (off_t)0);
	if ((long)fb->fb_mmapped == -1) {
		_ERR_
		printf_stderr("cannot mmap(fb_start)");
		return -1;
	}
	fb->fb_start = fb->fb_mmapped + fb->fb_offset;
#ifdef ENABLE_DEBUG
	verbose_printf("mmap : %p (fb_offset:%08lx, fb_mem_size:%08lx)\n",
	 fb->fb_mmapped, fb->fb_offset, fb->fb_mem_size);
	verbose_printf("fb_start: %p, fb_view_size:%08lx\n", fb->fb_start, fb->fb_view_size);
#endif // ENABLE_DEBUG
	verbose_printf("width(by line_length):%d, height(by smem_len):%d\n",
	 fb_fix.line_length / fb->bytes_per_pixel, fb->fb_mem_size / fb->bytes_per_line);
	if (app__.use_whole_buf
	 && (fb_fix.line_length / fb->bytes_per_pixel) > (fb->fb_mem_size / fb->bytes_per_line)) {
//		verbose_printf("width:%d, height:%d\n",
//		 fb_fix.line_length / fb->bytes_per_pixel, fb->fb_mem_size / fb->bytes_per_line);
		fb->width = fb_fix.line_length / fb->bytes_per_pixel;
		fb->height = fb->fb_mem_size / fb->bytes_per_line;
		verbose_printf("width:%d, height:%d\n", fb->width, fb->height);
	}

	/* move viewport to upper left corner */
	if (fb_var.xoffset != 0 || fb_var.yoffset != 0) {
		fb_var.xoffset = 0;
		fb_var.yoffset = 0;
		if (fb_pan_display(fb->fd_fb, &fb_var) < 0)
			return -1;
	}

	fb_setup_15_16_bpp_color_table();
	return 0;
}

int fb_close(frame_buffer_t *fb)
{
	int ret = 0;

	if ((long)fb->fb_mmapped == -1) {
		munmap((caddr_t)fb->fb_mmapped, fb->fb_mem_size);
	}
	if (color_map_saved == TRUE) {
///		fb_put_cmap(fb->fd_fb, &save_color_map);
		color_map_saved = FALSE;
	}
	if (var_screen_info_modified == TRUE) {
		save_fb_var.activate = FB_ACTIVATE_NOW;
		fb_set_var_screen_info(fb->fd_fb, &save_fb_var);
		var_screen_info_modified = FALSE;
	}
	if (fb->fd_fb >= 0) {
		if (close(fb->fd_fb) < 0) {
			ret = -1;
		}
	}
	if (fb->fd_tty0 >= 0) {
		if (close(fb->fd_tty0) < 0) {
			ret = -1;
		}
	}
	return ret;
}

PRIVATE int fb_select_driver(frame_buffer_t *fb,
 struct fb_var_screeninfo *fbvs,
 struct fb_fix_screeninfo *fbfs)
{
	int fb_drv_idx;
	int static_color = 0;

	if (fbfs->visual == FB_VISUAL_STATIC_PSEUDOCOLOR) {
		fbfs->visual = FB_VISUAL_PSEUDOCOLOR;
		static_color = 1;
	}
	flf_d_printf("FB bits per pixel: %d (r:%d, g:%d, b:%d)\n",
	 fbvs->bits_per_pixel,
	 fbvs->red.length, fbvs->green.length, fbvs->blue.length);
	for (fb_drv_idx = 0; fb_drivers[fb_drv_idx].bitsPerPixel; fb_drv_idx++) {
//flf_d_printf("FB type index: %d, bits per pixel: %d\n",
// fb_drv_idx, fb_drivers[fb_drv_idx].bitsPerPixel);
		if (fb_drivers[fb_drv_idx].bitsPerPixel == fbvs->bits_per_pixel) {
			if (static_color) {
				fbfs->visual = FB_VISUAL_STATIC_PSEUDOCOLOR;
			}
			return fb_drv_idx;
		}
	}
	for (fb_drv_idx = 0; fb_drivers[fb_drv_idx].bitsPerPixel; fb_drv_idx++) {
//flf_d_printf("FB type index: %d, bits per pixel: %d\n",
// fb_drv_idx, fb_drivers[fb_drv_idx].bitsPerPixel);
		if (fb_drivers[fb_drv_idx].bitsPerPixel
		 == fbvs->red.length + fbvs->green.length + fbvs->blue.length) {
			if (static_color) {
				fbfs->visual = FB_VISUAL_STATIC_PSEUDOCOLOR;
			}
			return fb_drv_idx;
		}
	}
	if (static_color) {
		fbfs->visual = FB_VISUAL_STATIC_PSEUDOCOLOR;
	}
	return -1;
}

#ifdef ENABLE_DEBUG
PRIVATE void fb_show_screeninfo(frame_buffer_t *fb,
 struct fb_var_screeninfo *fbvs, struct fb_fix_screeninfo *fbfs)
{
	int type, visual;
	const char *type_name;
	const char *vis_name; 

	type = fbfs->type;
	type_name =
		type == FB_TYPE_PACKED_PIXELS ?			"Packed Pixels" :
		type == FB_TYPE_PLANES ?				"Non interleaved planes" :
		type == FB_TYPE_INTERLEAVED_PLANES ?	"Interleaved planes" :
		type == FB_TYPE_TEXT ?					"Text/attributes" :
												"Unknown planes";
	visual = fbfs->visual;
	vis_name =
		visual == FB_VISUAL_MONO01 ?				"Monochr. 1=Black 0=White" :
		visual == FB_VISUAL_MONO10 ?				"Monochr. 1=White 0=Black" :
		visual == FB_VISUAL_TRUECOLOR ?				"True color" :
		visual == FB_VISUAL_PSEUDOCOLOR ?			"Pseudo color (like atari)" :
		visual == FB_VISUAL_DIRECTCOLOR ?			"Direct colo" :
		visual == FB_VISUAL_STATIC_PSEUDOCOLOR ?	"Pseudo color readonly" :
													"Unknown Visual";
	verbose_printf("===== Framebuffer Info ==========================\n");
	verbose_printf("NAME   : [%s]\n", fbfs->id);
	verbose_printf("TYPE   : %s\n", type_name);
	verbose_printf("VISUAL : %s\n", vis_name);
	verbose_printf("SMEM   : %p L=%u\n", fbfs->smem_start, fbfs->smem_len);
	verbose_printf("LLEN   : %u\n", fbfs->line_length);
	verbose_printf("RESO   : %ux%u+%u+%u / %ux%u @ %u\n",
	 fbvs->xres, fbvs->yres, fbvs->xoffset, fbvs->yoffset,
	 fbvs->xres_virtual, fbvs->yres_virtual,
	 fbvs->bits_per_pixel);
	verbose_printf("RED    : %u @ %u %c\n",
	 fbvs->red.offset, fbvs->red.length,
	 fbvs->red.msb_right ? '-' : '+');
	verbose_printf("GREEN  : %u @ %u %c\n",
	 fbvs->green.offset, fbvs->green.length,
	 fbvs->green.msb_right ? '-' : '+');
	verbose_printf("BLUE   : %u @ %u %c\n",
	 fbvs->blue.offset, fbvs->blue.length,
	 fbvs->blue.msb_right ? '-' : '+');
	verbose_printf("==================================================\n");
#if __BYTE_ORDER == __BIG_ENDIAN
	verbose_printf("CPU endian : BIG\n");
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	verbose_printf("CPU endian : LITTLE\n");
#else
#	error No endianness defined
#endif
}
#endif // ENABLE_DEBUG

PRIVATE void fb_setup_15_16_bpp_color_table(void)
{
	int color_idx;
	u_int32 argb32;

	for (color_idx = 0 ; color_idx < COLORS_16 ; color_idx++) {
		argb32 = table_16_color_argb32[color_idx];
		table_16_color_rgb16[color_idx] = rgb16_from_argb32(argb32);
		table_16_color_rgb15[color_idx] = rgb15_from_argb32(argb32);
	}
}
//-----------------------------------------------------------------------------
// rgb15_t <== color_idx
rgb15_t rgb15_from_color_idx(c_idx_t color_idx)
{
	return table_16_color_rgb15[MK_IN_RANGE(0, color_idx, COLORS_16)];
}
// rgb16_t <== color_idx
rgb16_t rgb16_from_color_idx(c_idx_t color_idx)
{
	return table_16_color_rgb16[MK_IN_RANGE(0, color_idx, COLORS_16)];
}
// argb32_t <== color_idx
argb32_t argb32_from_color_idx(c_idx_t color_idx)
{
	return table_16_color_argb32[MK_IN_RANGE(0, color_idx, COLORS_16)];
}
//-----------------------------------------------------------------------------
// r, g, b <== color_idx
void r_g_b_24_from_color_idx(c_idx_t color_idx, u_char *b0, u_char *b1, u_char *b2)
{
	r_g_b_24_from_argb32(argb32_from_color_idx(color_idx), b0, b1, b2);
}
// r, g, b <== rgb15_t
void r_g_b_24_from_rgb15(rgb15_t rgb15, u_char *b0, u_char *b1, u_char *b2)
{
	r_g_b_24_from_argb32(argb32_from_rgb15(rgb15), b0, b1, b2);
}
// r, g, b <== argb32_t
void r_g_b_24_from_argb32(argb32_t argb32, u_char *b0, u_char *b1, u_char *b2)
{
#if __BYTE_ORDER == __BIG_ENDIAN
	*b0 = argb32 >> 16;
	*b1 = argb32 >> 8;
	*b2 = argb32;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	*b0 = argb32;
	*b1 = argb32 >> 8;
	*b2 = argb32 >> 16;
#else
#	error No endianness defined
#endif
}
//-----------------------------------------------------------------------------
// rgb15_t <== r, g, b
rgb15_t rgb15_from_r_g_b_24(u_char rr, u_char gg, u_char bb)
{
	return rgb15_from_argb32(argb32_from_r_g_b_24(rr, gg, bb));
}
// rgb16_t <== r, g, b
rgb16_t rgb16_from_r_g_b_24(u_char rr, u_char gg, u_char bb)
{
	return rgb16_from_argb32(argb32_from_r_g_b_24(rr, gg, bb));
}
// argb32_t <== r, g, b
argb32_t argb32_from_r_g_b_24(u_char rr, u_char gg, u_char bb)
{
	return (((u_int32)rr) << 16) | (gg << 8) | bb;
}
//-----------------------------------------------------------------------------
// rgb16_t <== rgb15_t
rgb16_t rgb16_from_rgb15(rgb15_t rgb15)
{
	//     5432109876543210
	//     0RRRRRGGGGGBBBBB
	// ==> RRRRRGGGGGgBBBBB
	return ((rgb15 << 1) & 0xffe0) | (rgb15 & 0x1f);
}
// argb32_t <== rgb15_t
argb32_t argb32_from_rgb15(rgb15_t rgb15)
{
	//     10987654321098765432109876543210
	//                     0RRRRRGGGGGBBBBB
	// ==> aaaaaaaaRRRRRrrrGGGGGgggBBBBBbbb
	return ((((u_int32)rgb15) << 9) & 0x00f80000UL)
					| ((rgb15 << 6) & 0x0000f800UL)
					| ((rgb15 << 3) & 0x000000f8UL);
}
// rgb15_t <== rgb16_t
rgb15_t rgb15_from_rgb16(rgb16_t rgb16)
{
	//     5432109876543210
	//     RRRRRGGGGGgBBBBB
	// ==> 0RRRRRGGGGGBBBBB
	return ((rgb16 >> 1) & 0x7fe0) | (rgb16 & 0x1f);
}
// argb32_t <== rgb16_t
argb32_t argb32_from_rgb16(rgb15_t rgb16)
{
	//     10987654321098765432109876543210
	//                     RRRRRGGGGGGBBBBB
	// ==> aaaaaaaaRRRRRrrrGGGGGGggBBBBBbbb
	return ((((u_int32)rgb16) << 8) & 0x00f80000UL)
					| ((rgb16 << 5) & 0x0000fc00UL)
					| ((rgb16 << 3) & 0x000000f8UL);
}
// rgb15_t <== argb32_t
rgb15_t rgb15_from_argb32(argb32_t argb32)
{
	//     10987654321098765432109876543210
	//     aaaaaaaaRRRRRrrrGGGGGgggBBBBBbbb
	//     ==>             0RRRRRGGGGGBBBBB
	return ((argb32 & 0xf80000) >> 9)
		|  ((argb32 & 0x00f800) >> 6)
		|  ((argb32 & 0x0000f8) >> 3);
}
// rgb16_t <== argb32_t
rgb16_t rgb16_from_argb32(argb32_t argb32)
{
	//     10987654321098765432109876543210
	//     aaaaaaaaRRRRRrrrGGGGGGggBBBBBbbb
	//     ==>             RRRRRGGGGGGBBBBB
	return ((argb32 & 0xf80000) >> 8)
		|  ((argb32 & 0x00fc00) >> 5)
		|  ((argb32 & 0x0000f8) >> 3);
}

/*---------------------------------------------------------------------------*/

PRIVATE int fb_get_var_screen_info(int fd_fb, struct fb_var_screeninfo *var)
{
	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, var) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOGET_VSCREENINFO");
		return -1;
	}
	return 0;
}

PRIVATE int fb_set_var_screen_info(int fd_fb, struct fb_var_screeninfo *var)
{
	if (ioctl(fd_fb, FBIOPUT_VSCREENINFO, var) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOPUT_VSCREENINFO");
		return -1;
	}
	return 0;
}

PRIVATE int fb_get_fix_screen_info(int fd_fb, struct fb_fix_screeninfo *fix)
{
	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, fix) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOGET_FSCREENINFO");
		return -1;
	}
	return 0;
}

PRIVATE int fb_get_cmap(int fd_fb, struct fb_cmap *cmap)
{
	if (ioctl(fd_fb, FBIOGETCMAP, cmap) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOGETCMAP");
		return -1;
	}
	return 0;
}

PRIVATE int fb_put_cmap(int fd_fb, struct fb_cmap *cmap)
{
	if (ioctl(fd_fb, FBIOPUTCMAP, cmap) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOPUTCMAP");
		return -1;
	}
	return 0;
}

PRIVATE int fb_pan_display(int fd_fb, struct fb_var_screeninfo *var)
{
	if (ioctl(fd_fb, FBIOPAN_DISPLAY, var) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOPAN_DISPLAY");
		return -1;
	}
	return 0;
}

#if defined(ENABLE_DIMMER) && defined(FBIOBLANK)
int fb_set_blank(int fd_fb, int vt_num, int blank)
{
	struct vt_stat vt_st;

	if (ioctl(frame_buffer__.fd_tty0, VT_GETSTATE, &vt_st) < 0) {
		_ERR_
		printf_strerror("ioctl VT_GETSTATE");
		return 0;		/* can not get foreground console */
	}
	if (vt_st.v_active != vt_num) {
		return 0;		/* Not foreground console */
	}
	if (ioctl(fd_fb, FBIOBLANK, blank) < 0) {
		_ERR_
		printf_strerror("ioctl FBIOBLANK");
		return 0;
	}
	return 1;		// Did it.
}
#endif

const char *dump_rgb15(rgb15_t rgb15)
{
	static int buf_idx = 0;
	static char buffer[2][100+1];

	++buf_idx;
	buf_idx = buf_idx % 2;		// 0, 1
	snprintf(buffer[buf_idx], 100+1, "(%02x, %02x, %02x)",
	 ((rgb15 >> 10) & 0x1f),
	 ((rgb15 >>  5) & 0x1f),
	 ((rgb15 >>  0) & 0x1f));
	return buffer[buf_idx];
}

// End of fbcommon.c
