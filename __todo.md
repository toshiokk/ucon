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
| rotation = 0 | fb                               | + fbr_x_inc | + fbr_y_inc |
| rotation = 1 | fb + fbr_y_inc * (Yvram-1)        | - fbr_y_inc | + fbr_x_inc |
| rotation = 2 | fb + fbr_y_inc * Yvram - fbr_x_inc | - fbr_x_inc | - fbr_y_inc |
| rotation = 3 | fb + fbr_y_inc         - fbr_x_inc | + fbr_y_inc | - fbr_x_inc |

  Xview-max = Yvram-max
  Yview-max = Xvram-max

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
	ROT000 = 0,
	ROT090 = 1,
	ROT180 = 2,
	ROT270 = 3,
	ROT360 = 4,
};

### 4K support
- Xsize = 3840
- Ysize = 2160

##### EOF
