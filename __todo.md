## Display rotation in UCON

### Prerequisite
-  View-origin and Vram-origin is Top-Left.
- CW: Clock Wise
- CCW: Counter Clock Wise

| Parameter name | Meaning           | Example value       |
| -------------- | ----------------- | ------------------- |
| Xvram-size | Framebuffer screen X size | 3840                      |
| Yvram-size | Framabuffer screen Y size | 2160                      |
| Xview-size | View screen X size        | 2160 (on Rotation 90 deg) | 
| Yview-size | View screen Y size        | 3840 (on Rotation 90 deg) |

### Rotate CW 0, 90, 180, 270 degrees
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
~~~
- screen-rotation=1
~~~
*------------------------------+
|            ###########       |
|          ##  ##              |
|        ##    ##              |
|      ##      ##              |
|        ##    ##              |
|          ##  ##              |
|            ###########       |
+------------------------------+
+--------------*
|              |
|              |
|      ##      |
|     ####     |
|    ##  ##    |
|   ########   |
|  ##      ##  |
| ##        ## |
|##          ##|
|              |
|              |
+--------------+
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
+------------------------------+
|              ##              |
|             ####             |
|            ##  ##            |
|           ########           |
|          ##      ##          |
|         ##        ##         |
|        ##          ##        |
+------------------------------*
~~~
- screen-rotation=3
~~~
*------------------------------+
|      ############            |
|              ##  ##          |
|              ##    ##        |
|              ##      ##      |
|              ##    ##        |
|              ##  ##          |
|       ############           |
+------------------------------+
+--------------+
|              |
|              |
|      ##      |
|     ####     |
|    ##  ##    |
|   ########   |
|  ##      ##  |
| ##        ## |
|##          ##|
|              |
|              |
*--------------+
~~~

#### Screen rotation
| Screen<br>-rotation | Monitor<br> Rotation | Xview-size | Yview-size |
| -------------     | -------------------- | ---------- | ---------- |
| rotation = 0 | no rotation 0 deg | Xvram-size | Yvram-size |
| rotation = 1 | rotate CW  90 deg | Yvram-size | Xvram-size |
| rotation = 2 | rotate CW 180 deg | Xvram-size | Yvram-size |
| rotation = 3 | rotate CW 270 deg | Yvram-size | Xvram-size |

#### Coordinate conversion
| Screen<br>-rotation | get Xvram       | get Yvram                |
| -------------     | --------------- | ------------------------ |
| rotation = 0 | Xview                    | Yview                    |
| rotation = 1 | (Yvram-size - 1) - Yview | Xview                    |
| rotation = 2 | (Xvram-size - 1) - Xview | (Yvram-size - 1) - Yview | 
| rotation = 3 | Yview                    | (Xvram-size - 1) - Xview |

#### Text screen size
| Screen<br>-rotation | Text X chars | Text Y chars |
| ----------------- | ------------ | ------------ |
| rotation = 0 | Xvram-size / Xfont-size | Yvram-size / Yfont-size |
| rotation = 1 | Yvram-size / Xfont-size | Xvram-size / Yfont-size |
| rotation = 2 | Xvram-size / Xfont-size | Yvram-size / Yfont-size |
| rotation = 3 | Yvram-size / Xfont-size | Xvram-size / Yfont-size |

#### VRAM address calculation
| Screen<br>-rotation | VRAM origin | VRAM origin axis |
| ----------------- | ------------| ---------------- | 
| rotation = 0 | VRAM top-left     | (0, 0)             |
| rotation = 1 | VRAM bottom-left  | (0, Ysize-1)       | 
| rotation = 2 | VRAM bottom-right | (Xsize-1, Ysize-1) | 
| rotation = 3 | VRAM top-right    | (Xsize-1, 0)       | 

| Screen<br>-rotation | X increment<br>fb_x_inc | Y increment<br>fb_y_inc |
| ----------------- | ----------------------- | ----------------------- |
| rotation = 0 | Add pixel byte size      | Add line byte size       |
| rotation = 1 | Subtract line byte size  | Add pixel byte size      |
| rotation = 2 | Subtract pixel byte size | Subtract line byte size  |
| rotation = 3 | Add line byte size       | Subtract pixel byte size |

| Screen<br>-rotation | Screen origin address | X increment | Y increment |
| ----------------- | --------------------- | ----------- | ----------- |
| rotation = 0 | fb                               | + fb_x_inc | + fb_y_inc |
| rotation = 1 | fb + fb_y_inc * (Yvram-1)        | - fb_y_inc | + fb_x_inc |
| rotation = 2 | fb + fb_y_inc * Yvram - fb_x_inc | - fb_x_inc | - fb_y_inc |
| rotation = 3 | fb + fb_y_inc         - fb_x_inc | + fb_y_inc | - fb_x_inc |

  Xview-max = Yvram-max
  Yview-max = Xvram-max

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
                  Yvram = (Yvram-max-1) - Xview 
~~~

#### Parameter conversion

~~~
enum view_rotation_t {
	ROT0 = 0,
	ROT90 = 1,
	ROT180 = 2,
	ROT270 = 3,
	ROT360 = 4,
};

void *fb_begin;		// FB memory start address
size_t fb_size;		// FB memory size
fb_pixel_data_t = ulong;
	ushort fb_size_x;	// pixels of X
	ushort fb_size_y;	// pixels of Y
void *fb_top_left; 		// FB address of top_left
void *fb_bottom_left;		// FB address of bottom_left
void *fb_bottom_right;		// FB address of bottom_right
void *fb_top_right;		// FB address of top_right
size_t fb_pixel_bytes_x;
size_t fb_pixel_bytes_y;

	view_rotaion_t screen_rotation = 1;	// 0,1,2,3
	// rotation dependent parameters
	void *fb_origin;		// FB address of top_left
	ushort view_size_x;
	ushort view_size_y;
	ssize_t fb_x_inc;
	ssize_t fb_y_inc;

	// font size dependent parameters
	ssize_t fb_font_x_inc;
	ssize_t fb_font_y_inc;
	ushort text_chars_x;
	ushort text_chars_y;

void setup_rotation()
{
	fb_pixel_bytes_x = sizeof(fb_pixel_data_t);
	fb_pixel_bytes_y = fb_pixel_bytes_x * fb_size_x;

	fb_top_left = fb_begin
	 + fb_pixel_bytes_x * (fb_size_x - 1) * 0
	 + fb_pixel_bytes_y * (fb_size_y - 1) * 0;
	fb_bottom_left = fb_begin
	 + fb_pixel_bytes_x * (fb_size_x - 1) * 0
	 + fb_pixel_bytes_y * (fb_size_y - 1) * 1;
	fb_bottom_right = fb_begin
	 + fb_pixel_bytes_x * (fb_size_x - 1) * 1
	 + fb_pixel_bytes_y * (fb_size_y - 1) * 1;
	fb_top_right = fb_begin
	 + fb_pixel_bytes_x * (fb_size_x - 1) * 1
	 + fb_pixel_bytes_y * (fb_size_y - 1) * 0;

	switch (screen_rotation) {
	case ROT0:
	case ROT180:
		view_size_x = fb_size_x;
		view_size_y = fb_size_y;
		break;
	case ROT90:
	case ROT270:
		view_size_x = fb_size_y;
		view_size_y = fb_size_x;
		break;
	}
	text_chars_x = view_size_x / font_size_x;
	text_chars_y = view_size_y / font_size_y;

	switch (screen_rotation) {
	case ROT0:
		fb_origin = fb_top_left;
		fb_x_inc = + fb_pixel_bytes_x;
		fb_y_inc = + fb_pixel_bytes_y;
		break;
	case ROT180:
		fb_origin = fb_bottom_left;
		fb_x_inc = - fb_pixel_bytes_x;
		fb_y_inc = - fb_pixel_bytes_y;
		break;
	case ROT90:
		fb_origin = fb_bottom_right;
		fb_x_inc = - fb_pixel_bytes_y;
		fb_y_inc = + fb_pixel_bytes_x;
		break;
	case ROT270:
		fb_origin = fb_top_right;
		fb_x_inc = + fb_pixel_bytes_y;
		fb_y_inc = - fb_pixel_bytes_x;
		break;
	}
	fb_font_x_inc = fb_x_inc * font_size_x;
	fb_font_y_inc = fb_y_inc * font_size_y;
}
~~~

#### Fast box-paint
~~~
int box_paint(ushort text_x, ushort text_y, uchar font_multi_x, uchar font_multi_y,
 uchar *font, uchar font_size_x, uchar font_size_y, uchar fgc, uchar bgc)
	fb_pixel_data_t fb_fgc = argb32_from_fgc(fgc);
	fb_pixel_data_t fb_bgc = argb32_from_fgc(bgc);

	fb_pixel_data_t *fb_left;
	fb_pixel_data_t *fb;
	uchar font_byte;
	uchar font_bit;
	fb_left = (fb_pixel_data_t *)(fb_origin
	 + text_y * font_multi_y * fb_font_y_inc
	 + text_x * font_multi_y * fb_font_x_inc);
	for (uchar font_y = 0; font_y < font_size_y; font_y++) {
		for (uchar multi_y = 0; multi_y < font_multi_y; multi_y++) {
			fb = fb_left;
			font_byte = *font++;
			font_bit = 0x80;
			for (uchar font_x = 0; font_x < font_size_x; font_x++) {
				for (uchar multi_x = 0; multi_x < font_multi_x; multi_x++) {
					*(fb_pixel_data_t *)fb = ((font_byte & font_bit) ? fb_fgc : fb_bgc);
					// paint
					fb += fb_x_inc;
				}
				font++;
			}
			fb_left += fb_y_inc;
		}
	}
}
~~~

### 4K support
- Xsize = 3840
- Ysize = 2160


##### EOF
