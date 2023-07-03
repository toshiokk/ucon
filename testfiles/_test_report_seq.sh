/bin/echo -n "echo terminal status by ESC [ 0 n ==> "
/bin/echo -ne "\033[5n"
sleep 1
/bin/echo

/bin/echo -n "echo device code by ESC [ {code} 0 c ==> "
/bin/echo -ne "\033[0c"
sleep 1

/bin/echo ""
/bin/echo -n "echo current cursor position by ESC [ y ; x R ==> "
/bin/echo -ne "\033[6n"
/bin/echo
sleep 1

/bin/echo -n "echo current cursor position by ESC [ y ; x R ==> "
/bin/echo -ne "\033[999;999H"
sleep 1
/bin/echo -ne "\033[6n"
/bin/echo
sleep 3

/bin/echo -n "pause: "; read key
