## UCON - Linux Framebuffer based UTF8 Terminal with Japanese Input Method.

### Overview
- UCON is Framabuffer based UTF8 Terminal.
    - UCON render characters onto Linux framebuffer.
    - UTF8 support.
    - Includes 4 sizes of bitmap fonts in HEX font file format.
        - 5x10 ASCII, 10x10 Unicode font
        - 6x12 ASCII, 12x12 Unicode font
        - 7x14 ASCII, 14x14 Unicode font
        - 8x16 ASCII, 16x16 Unicode font
    - Built in Japanese IME(Input Method) with canna dictionary.
    - Screen rotation support (You can use your monitor as "portrait").

### What is UCON
- Started based on JFBTERM.
- JFBTERM
    - Copyright (C) 2003 Fumitoshi UKAI
    - Copyright (C) 1999-2000 Noritoshi Masuichi
    - JFBTERM is based on KON2
- KON2
    - Copyright (C) 1992-1996 Takashi MANABE

### Data flow of Framebuffer Console
~~~
/dev/console
 |  +==================+                    +------------------+
 v  |       ucon       |   +-----------+    |    console based |
 |  |                  |   |           |    |      application |
 \->|fd:0----->[imj]-->|-->|    pty    |--->|fd:0  (ex. shell) |
    |                  |   | interface |    |                  |
    |                  |<--|           |<-+-|fd:1              |
    |                  |   +-----------+  +-|fd:2              |
    +==================+                    +------------------+
            |||||
            vvvvv render characters onto Framebuffer
        +-----------+
        |           |
        |    FB     |
        |640x480etc.|
        |           |
        +-----------+
~~~
