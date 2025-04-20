/*
 * UCON -
 * Copyright (C) 2013 Toshio Koike
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
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
 */

#ifndef fbrotation_h
#define fbrotation_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

typedef enum view_rotation_ {
	ROT000 = 0,		// No rotation on screen
	ROT090 = 1,		// Rotate contents  90 [Degrees] CCW on screen
	ROT180 = 2,		// Rotate contents 180 [Degrees] CCW on screen
	ROT270 = 3,		// Rotate contents 270 [Degrees] CCW on screen
	ROT360 = 4,		// Modulo of rotation index
} view_rotation_t;
extern view_rotation_t contents_rotation;

void fbr_setup_constant_parameters();
void fbr_copy_font_size_into_driver();
void fbr_set_rotation(view_rotation_t rotation);
view_rotation_t fbr_get_rotation();
void fbr_setup_rotation_dependent_parameters();
void fbr_set_text_metrics();

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // fbrotation_h

// End of fbrotation.h
