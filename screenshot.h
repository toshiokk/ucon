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

#ifndef ucon_screenshot_h
#define ucon_screenshot_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

int screen_shot(int size_x, int size_y,
 argb32_t (*get_pixel_argb32)(int xx, int yy), void (*reverse_all)(void),
 const char *file_name);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // ucon_screenshot_h

// End of screenshot.h
