[Name]
jfbterm kon ufbterm utfterm utf8term uterm ucon

ucon - UTF-8 Frame Buffer Console with Japanese Input Method
ucon - 日本語入力機能内蔵UTF-8フレームバッファコンソール

-Save display image for IM-input-line(the next line of cursor)
-Inputting YOMI and converting to Kanji in the IM-input-line
-Restore display image and send Kanji characters to application

TODO:
[]予測変換「よ」→「よろしく」、「よろしくお願い致します」
　読み入力中に予測変換候補が表示される。
　↓キ－で予測変換候補から選択可能。

[x]IM-ON時、1文字目のスペースキー入力は全角スペースを出力する。

[x]workaround for Switching VT
　自分にVTがスイッチされて描画しても画面が表示されない不具合を回避。
　==>一旦他のTEXTモードのVTにスイッチして自分にスイッチすると表示される。
[x]IMJの変換候補表示のちらつき防止

[x]禁忌識別子の修正\b_[:alpha:] __

英語の原義
 refresh: fresh(新鮮な)状態にする。最新の状態にする。
 update: 最新の状態にする。更新する。

コンピュータ用語として
 refresh/update 書き換えられた仮装スクリーンの状態をFBに反映する
	書き換えられた仮装スクリーンをFBに反映する事が目的
 show スクリーン上に描画してよい状態、または描画してはいけない状態にする。
 hide 実際の描画は好ましいタイミングで行われる。
 paint 仮装スクリーンの状態をFBに(再)描画する
 repaint 他のソフト等からFBが書き換えられて表示が乱れた場合等に再描画する
	仮装スクリーンが書き換えられた場合ではなく、乱れたFBを修復するのが主目的

open/close
create/destroy
construct/destruct
init/reinit

[dx]on-the-fly font size switching
[x]screen-shot to PNG file
//* remove other encodings than UTF-8
//* refrect IM on/off state to cursor color and blinking speed
//* zip compressed HEX font file support(unifont.hex.gz)
//* beep
//  ==> redirect BEL code to original stdout
//* zero-conf
//  no need of configuration file
//  5x10,6x12,7x14 font support
//  Workaround for i810/815/845/855 Framebuffer image corruption
//  Reduce flickering in drawing character
//  Gracefully die on kill signal
  Cannot run Qt-embedded properly
	can not input from KB
	not hidden text cursor

History:
100302 Changed project name from ufbterm to ucon.

[]
pixels  font chars
640x480 8x16 80x30
640x480 7x14 91x34
640x480 6x12 106x40
640x480 5x10 128x48

BUGS:
//Hung up on execution of "top" command
-------------------------------------------------------------------------------------------------------------------------------
===============================================================================================================================
