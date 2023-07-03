# UCON - Linux Framebuffer based UTF8 Terminal with Japanese Input Method.

## Overview
- UCON is Framabuffer based UTF8 Terminal.
    - UCON render charactors onto Linux framebuffer.
    - UTF8 support.
    - Includes 4 types of bitmap fonts
        - 6x12 ASCII, 12x12 Unicode font
        - 7x14 ASCII, 14x14 Unicode font
        - 8x16 ASCII, 16x16 Unicode font
    - Built in Japanese IME(Input Method) includes canna dictionary.

## What is BE editor:
- Started based on JFBTERM.
- JFBTERM is based on and forked from GNU NANO editor 1.2.2 (2003).
    - Copyright (C) 2003 Fumitoshi UKAI
    - Copyright (C) 1999-2000 Noritoshi Masuichi
- JFBTERM is based on KON2
    - Copyright (C) 1992-1996 Takashi MANABE

## Data flow of Framebuffer Console
~~~
				+===================+					+------------------+
				|		ucon		|	+-----------+	|    console based |
				|					|   |           |   |      application |
 /dev/console-->|fd:0----->[imj]--->|-->|    pty    |-->|fd:0  (ex. shell) |
				|					|   | interface |   |                  |
				|					|<--|           |<+-|fd:1              |
				|					|	+-----------+ +-|fd:2              |
				+===================+					+------------------+
						|||||
						vvvvv render characters onto Framebuffer
					+-----------+
					|			|
					|    FB		|
					|640x480etc.|
					|			|
					+-----------+
~~~
