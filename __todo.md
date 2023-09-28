## 4K support

## Rotate CW(Clock Wise) 0, 90, 180, 270 degrees
Rotate display monitor Right 90 deg (Rotate output of image Left 90 deg):

| Rotation<br>-mode | Monitor<br> Rotation | X size | Y size | Xvram                | Yvram                    |
| -------------     | -------- | ------     | ------     | -----                    | -----                    |
| mode = 0 | no rotation 0 deg | Xvram-size | Yvram-size | Xview                    | Yview                    |
| mode = 1 | rotate CW  90 deg | Yvram-size | Xvram-size | (Yvram-size - 1) - Yview | Xview                    |
| mode = 2 | rotate CW 180 deg | Xvram-size | Yvram-size | (Xvram-size - 1) - Xview | (Yvram-size - 1) - Yview | 
| mode = 3 | rotate CW 270 deg | Yvram-size | Xvram-size | Yview                    | (Xvram-size - 1) - Xview |

Text size
| Rotation<br>-mode | Text X chars | Text Y chars |
| -------------     | ------       | ------       |
| mode = 0 | Xvram-size / Xfont-size | Yvram-size / Yfont-size |
| mode = 1 | Yvram-size / Xfont-size | Xvram-size / Yfont-size |
| mode = 2 | Xvram-size / Xfont-size | Yvram-size / Yfont-size |
| mode = 3 | Yvram-size / Xfont-size | Xvram-size / Yfont-size |

VRAM address calculation
| Rotation<br>-mode | Start VRAM address | X increment | Y increment |
| ----------------- | ------------------ | ----------- | ----------- | 
| mode = 0 | Calc. from Xvram, Yvram | Add pixel byte size      | Add line byte size       |
| mode = 1 | Calc. from Xvram, Yvram | Subtract line byte size  | Add pixel byte size      |
| mode = 2 | Calc. from Xvram, Yvram | Subtract pixel byte size | Subtract line byte size  |
| mode = 3 | Calc. from Xvram, Yvram | Add line byte size       | Subtract pixel byte size |

  Xtext-max = Yvram-max / Xfontsize, Ytext-max = Xvram-max / Yfontsize
  View-origin and Vram-origin is Top-Left.
  Xview-max = Yvram-max, Yview-max = Xvram-max

  (Xvram)   (  0 1 ) (Xview)   (0        )   (  Yview                 )
  (Yvram) = ( -1 0 ) (Yview) + (Yvram-max) = ( -Xview + (Yvram-max-1) )
  Xvram =  0 * Xview + 1 * Yview + 0         =  Yview       
  Yvram = -1 * Xview + 0 * Yview + Yvram-max = -Xview + Ymax

  Ex.1 FHD rotate 90 deg
~~~
    Physical-size Xvram-max = 1920
                  Yvram-max = 1080
    User-size     Xview-max = Yvram-max = 1080
                  Yview-max = Xvram-max = 1920
    Xvram = Yview, Yvram = -Xview + 1080
~~~

### Fast box-paint
~~~
   fb_begin, fb_end
   fb_x_inc, fb_y_inc

   fb = fb_begin;
   for (font_y to font_y + font_sy) {
       for (font_x to font_x + font_sx) {
           // paint
           fb += fb_x_inc;
       }
       fb += fb_y_inc;
   }
~~~


##### EOF
