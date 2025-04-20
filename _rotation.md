## Display rotation in UCON

### ### Prerequisite
-  View-origin and Vram-origin is Top-Left.
- CW: Clock Wise
- CCW: Counter Clock Wise

| Parameter name | Meaning           | Example value       |
| -------------- | ----------------- | ------------------- |
| Xvram-size | Framebuffer screen X size | 3840                      |
| Yvram-size | Framabuffer screen Y size | 2160                      |
| Xview-size | View screen X size        | 2160 (on Rotation 90 deg) | 
| Yview-size | View screen Y size        | 3840 (on Rotation 90 deg) |

### ### Rotate CW 0, 90, 180, 270 degrees
Rotate display monitor CW 90 deg (Rotate output of image CCW 90 deg).
- screen-rotation=0
~~~
*------------------------------+
|              ##              |
|             ####             |
|            ##  ##            |
|           ########           |
|          ##      ##          |
|         ##        ##         |
|        ##          ##        |
+------------------------------+
*---+
| # |
|# #|
|###|
|# #|
|# #|
+---+
~~~
- screen-rotation=1
~~~
*------------------------------+
|            ##########        |
|          ##  ##              |
|        ##    ##              |
|      ##      ##              |
|        ##    ##              |
|          ##  ##              |
|            ##########        |
+------------------------------+
*-----+
| ####|
|# #  |
| ####|
+-----+
~~~
- screen-rotation=2
~~~
*------------------------------+
|        ##          ##        |
|         ##        ##         |
|          ##      ##          |
|           ########           |
|            ##  ##            |
|             ####             |
|              ##              |
+------------------------------+
*---+
|# #|
|# #|
|###|
|# #|
| # |
+---+
~~~
- screen-rotation=3
~~~
*------------------------------+
|        ##########            |
|              ##  ##          |
|              ##    ##        |
|              ##      ##      |
|              ##    ##        |
|              ##  ##          |
|        ###########           |
+------------------------------+
*-----+
|#### |
|  # #|
|#### |
+-----+
~~~

#### Screen rotation
| Screen<br>-rotation | Monitor<br> Rotation | Xview-size | Yview-size |
| ------------------- | -------------------- | ---------- | ---------- |
| rotation = 0 | no rotation 0 deg | Xvram-size | Yvram-size |
| rotation = 1 | rotate CW  90 deg | Yvram-size | Xvram-size |
| rotation = 2 | rotate CW 180 deg | Xvram-size | Yvram-size |
| rotation = 3 | rotate CW 270 deg | Yvram-size | Xvram-size |

#### Coordinate conversion
| Screen<br>-rotation | get Xvram       | get Yvram                |
| ---------------     | --------------- | ------------------------ |
| rotation = 0 | Xview                    | Yview                    |
| rotation = 1 | (Yvram-size - 1) - Yview | Xview                    |
| rotation = 2 | (Xvram-size - 1) - Xview | (Yvram-size - 1) - Yview | 
| rotation = 3 | Yview                    | (Xvram-size - 1) - Xview |

#### Text screen size
| Screen<br>-rotation | Text X chars | Text Y chars |
| ------------------- | ------------ | ------------ |
| rotation = 0 | Xvram-size / Xfont-size | Yvram-size / Yfont-size |
| rotation = 1 | Yvram-size / Xfont-size | Xvram-size / Yfont-size |
| rotation = 2 | Xvram-size / Xfont-size | Yvram-size / Yfont-size |
| rotation = 3 | Yvram-size / Xfont-size | Xvram-size / Yfont-size |

#### VRAM address calculation
| Screen<br>-rotation | VRAM origin | VRAM origin axis |
| ------------------- | ------------| ---------------- | 
| rotation = 0 | VRAM top-left     | (0, 0)             |
| rotation = 1 | VRAM bottom-left  | (0, Ysize-1)       | 
| rotation = 2 | VRAM bottom-right | (Xsize-1, Ysize-1) | 
| rotation = 3 | VRAM top-right    | (Xsize-1, 0)       | 

| Screen<br>-rotation | X increment<br>fbr_x_inc | Y increment<br>fbr_y_inc |
| ------------------- | ----------------------- | ----------------------- |
| rotation = 0 | Add pixel byte size      | Add line byte size       |
| rotation = 1 | Subtract line byte size  | Add pixel byte size      |
| rotation = 2 | Subtract pixel byte size | Subtract line byte size  |
| rotation = 3 | Add line byte size       | Subtract pixel byte size |

| Screen<br>-rotation | Screen origin address | X increment | Y increment |
| ------------------- | --------------------- | ----------- | ----------- |
| rotation = 0 | fb                                 | + fbr_x_inc | + fbr_y_inc |
| rotation = 1 | fb + fbr_y_inc * (Yvram-1)         | - fbr_y_inc | + fbr_x_inc |
| rotation = 2 | fb + fbr_y_inc * Yvram - fbr_x_inc | - fbr_x_inc | - fbr_y_inc |
| rotation = 3 | fb + fbr_y_inc         - fbr_x_inc | + fbr_y_inc | - fbr_x_inc |

~~~
  (Xvram)   (  0 1 ) (Xview)   (0        )   (  Yview                 )
  (Yvram) = ( -1 0 ) (Yview) + (Yvram-max) = ( -Xview + (Yvram-max-1) )
  Xvram =  0 * Xview + 1 * Yview + 0         =  Yview       
  Yvram = -1 * Xview + 0 * Yview + Yvram-max = -Xview + Ymax
~~~

####  Ex.1 FHD rotate 90 deg
~~~
    Physical-size Xvram-max = 1920
                  Yvram-max = 1080
    User-size     Xview-max = Yvram-max = 1080
                  Yview-max = Xvram-max = 1920
    Axis conversion from View to Vram
                  Xvram = Yview
                  Yvram = (Yvram-max - 1) - Xview 
~~~

#### Parameter conversion

~~~
enum view_rotation_t {
	ROT000 = 0,
	ROT090 = 1,
	ROT180 = 2,
	ROT270 = 3,
	ROT360 = 4,
};

void *fb_begin;		// FB memory start address
size_t fb_size;		// FB memory size
fb_pixel_data_t = ulong;
ushort fb_size_x;	// pixels of X (640 -- 3840)
ushort fb_size_y;	// pixels of Y (480 -- 1920)
void *fb_top_left; 	// FB address of top_left
void *fb_top_right;	// FB address of top_right
void *fb_bottom_left;	// FB address of bottom_left
void *fb_bottom_right;	// FB address of bottom_right

size_t fb_pixel_byte_inc_x;
size_t fb_pixel_byte_inc_y;

uchar font_size_x;	// size of font x
uchar font size_y;	// size of font y
uchar font_mul_x;	// expansion of font x
uchar font_mul_y;	// expansion of font y

view_rotation_t screen_rotation = 1;	// 0,1,2,3
// rotation dependent parameters
void *fbr_origin;	// FB address of top_left
ushort fbr_view_size_x;	// 640/480...3840/1920
ushort fbr_view_size_y;	// 480/640...1920/3840
ssize_t fbr_x_inc;	// 4 / 640x4
ssize_t fbr_y_inc;	// 640x4 / 4

// font size dependent parameters
ssize_t fb_fx_inc;
ssize_t fb_fy_inc;
ushort text_chars_x;
ushort text_chars_y;

void setup_rotation()
{
	fb_pixel_byte_inc_x = sizeof(fb_pixel_data_t);
	fb_pixel_byte_inc_y = fb_pixel_byte_inc_x * fb_size_x;

	fb_top_left = fb_begin
	 + fb_pixel_byte_inc_x * (fb_size_x - 1) * 0
	 + fb_pixel_byte_inc_y * (fb_size_y - 1) * 0;
	fb_bottom_left = fb_begin
	 + fb_pixel_byte_inc_x * (fb_size_x - 1) * 0
	 + fb_pixel_byte_inc_y * (fb_size_y - 1) * 1;
	fb_bottom_right = fb_begin
	 + fb_pixel_byte_inc_x * (fb_size_x - 1) * 1
	 + fb_pixel_byte_inc_y * (fb_size_y - 1) * 1;
	fb_top_right = fb_begin
	 + fb_pixel_byte_inc_x * (fb_size_x - 1) * 1
	 + fb_pixel_byte_inc_y * (fb_size_y - 1) * 0;

	switch (screen_rotation) {
	case ROT000:
	case ROT180:
		fbr_view_size_x = fb_size_x;
		fbr_view_size_y = fb_size_y;
		break;
	case ROT090:
	case ROT270:
		fbr_view_size_x = fb_size_y;
		fbr_view_size_y = fb_size_x;
		break;
	}
	text_chars_x = fbr_view_size_x / font_size_x;
	text_chars_y = fbr_view_size_y / font_size_y;

	switch (screen_rotation) {
	case ROT000:
		fbr_origin = fb_top_left;
		fbr_pixel_byte_inc_x = + fb_pixel_byte_inc_x;
		fbr_pixel_byte_inc_y = + fb_pixel_byte_inc_y;
		break;
	case ROT180:
		fbr_origin = fb_bottom_left;
		fbr_pixel_byte_inc_x = - fb_pixel_byte_inc_x;
		fbr_pixel_byte_inc_y = - fb_pixel_byte_inc_y;
		break;
	case ROT090:
		fbr_origin = fb_bottom_right;
		fbr_pixel_byte_inc_x = - fb_pixel_byte_inc_y;
		fbr_pixel_byte_inc_y = + fb_pixel_byte_inc_x;
		break;
	case ROT270:
		fbr_origin = fb_top_right;
		fbr_pixel_byte_inc_x = + fb_pixel_byte_inc_y;
		fbr_pixel_byte_inc_y = - fb_pixel_byte_inc_x;
		break;
	}
}
~~~

### 4K support
- Xsize = 3840
- Ysize = 2160

### ### goals
- XX direct rendering of glyph
- OO FB address calculation and paint glyph to the address

- 1st step:
	- implement strait (w/o speed considerations)
- 2nd step:
	- improve speed (with speed considerations)

### ### important naming convention
- fb_...   parameters in a original screen coordinate (without rotation)
- fbr_...  parameters in a view screen coordinate (with rotation)

fbx, fby: fb-x, fb-y (in original screen rotation)
vx, vy: view-x, view-y (in screen rotation)
px, py: pixel-position
cx, cy: charactor-position cx{0--79}, cy{0--24}
bytes_inc_x
bytes_inc_y

// rotation: 0      1        2      3
//           *---+  *-----+  *---+  *-----+
//           | # |  | ####|  |# #|  |#### |
//           |# #|  |# #  |  |# #|  |  # #|
//           |###|  | ####|  |###|  |#### |
//           |# #|  +-----+  |# #|  +-----+
//           |# #|           | # |         
//           +---+           +---+         

fx: font x
fy: font y
FX: font size of x
FY: font size of y


u_char fbr_fx_from_fbx_forw;
u_char fbr_fx_from_fbx_rev;
u_char fbr_font_sx_m1;
u_char fbr_fx_from_fby_forw;
u_char fbr_fx_from_fby_rev;
u_char fbr_font_sx_m1;

u_char fbr_fy_from_fby_forw;
u_char fbr_fy_from_fby_rev;
u_char fbr_font_sy_m1;
u_char fbr_fy_from_fbx_forw;
u_char fbr_fy_from_fbx_rev;
u_char fbr_font_sy_m1;

u_char begin_get_glyph_pixel_rot(UCS21 ucs21, bool wide)
{
	begin_get_glyph_pixel(ucs21, wide);
	switch (rotation) {
	case ROT000:
		fbr_fx_from_fbx_forw = 1;
		fbr_fx_from_fbx_rev = 0;
		fbr_fx_from_fby_forw = 0;
		fbr_fx_from_fby_rev = 0;
		fbr_fy_from_fby_forw = 1;
		fbr_fy_from_fby_rev = 0;
		fbr_fy_from_fbx_forw = 0;
		fbr_fy_from_fbx_rev = 0;
		fbr_font_sx_m1 = fbr_font_size_x + fbr_font_size_x * wide - 1;
		fbr_font_sy_m1 = fbr_font_size_y - 1;
		break;
	case ROT090:
		fbr_fx_from_fbx_forw = 0;
		fbr_fx_from_fbx_rev = 1;
		fbr_fx_from_fby_forw = 0;
		fbr_fx_from_fby_rev = 0;
		fbr_fy_from_fby_forw = 0;
		fbr_fy_from_fby_rev = 1;
		fbr_fy_from_fbx_forw = 0;
		fbr_fy_from_fbx_rev = 0;
		fbr_font_sx_m1 = fbr_font_size_x + fbr_font_size_x * wide - 1;
		fbr_font_sy_m1 = fbr_font_size_y - 1;
		break;
	case ROT180:
		fbr_fx_from_fbx_forw = 0;
		fbr_fx_from_fbx_rev = 0;
		fbr_fx_from_fby_forw = 1;
		fbr_fx_from_fby_rev = 0;
		fbr_fy_from_fby_forw = 0;
		fbr_fy_from_fby_rev = 0;
		fbr_fy_from_fbx_forw = 1;
		fbr_fy_from_fbx_rev = 0;
		fbr_font_sx_m1 = fbr_font_size_x + fbr_font_size_x * wide - 1;
		fbr_font_sy_m1 = fbr_font_size_y - 1;
		break;
	case ROT270:
		fbr_fx_from_fbx_forw = 0;
		fbr_fx_from_fbx_rev = 0;
		fbr_fx_from_fby_forw = 0;
		fbr_fx_from_fby_rev = 1;
		fbr_fy_from_fby_forw = 0;
		fbr_fy_from_fby_rev = 0;
		fbr_fy_from_fbx_forw = 0;
		fbr_fy_from_fbx_rev = 1;
		fbr_font_sx_m1 = fbr_font_size_x + fbr_font_size_x * wide - 1;
		fbr_font_sy_m1 = fbr_font_size_y - 1;
		break;
	}
}
inline u_short get_glyph_pixel_rot(u_char fbx, u_char fby)
{
  u_char fx =
    + fx_from_fbx_forw(fbx)
    + fx_from_fbx_rev(fbx)
    + fx_from_fby_forw(fby)
    + fx_from_fby_rev(fby)
  ;
  u_char fy =
    + fy_from_fbx_forw(fbx)
    + fy_from_fbx_rev(fbx)
    + fy_from_fby_forw(fby)
    + fy_from_fby_rev(fby)
  ;
  return get_glyph_pixel(fx, fy);
}

inline u_short fx_from_fbx_forw(u_short fbx)
{
  return fbr_fx_from_fbx_forw * fbx;
}
inline u_short fx_from_fbx_rev(u_short fbx)
{
  return fbr_fx_from_fbx_rev * (fbr_font_sx_m1 - fbx);
}
inline u_short fx_from_fby_forw(u_short fby)
{
  return fbr_fx_from_fby_forw * fby;
}
inline u_short fx_from_fby_rev(u_short fby)
{
  return fbr_fx_from_fby_rev * (fbr_font_sy_m1 - fby);
}

inline u_short fy_from_fby_forw(u_short fby)
{
  return fbr_fy_from_fby_forw * fby;
}
inline u_short fy_from_fby_rev(u_short fby)
{
  return fbr_fy_from_fby_rev * (fbr_font_sy_m1 - fby);
}
inline u_short fy_from_fbx_forw(u_short fbx)
{
  return fbr_fy_from_fbx_forw * fbx;
}
inline u_short fy_from_fbx_rev(u_short fbx)
{
  return fbr_fy_from_fbx_rev * (fbr_font_sx_m1 - fbx);
}


PRIVATE u_short *glyph = font_get_glyph_bitmap(0x0000, 0);

u_char begin_get_glyph_pixel(UCS21 ucs21, bool wide)
{
  glyph = font_get_glyph_bitmap(ucs21, wide);
}
u_short get_glyph_pixel(u_char fx, u_char fy)
{
  return (glyph[fy] >> fx) && 0x8000; // 0x8000: ON, 0x0000: OFF
}

##### EOF
