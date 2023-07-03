/bin/echo "cursor home"
sleep 1
/bin/echo -ne "\e[1;1H"
sleep 1

/bin/echo "Backspace"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1

/bin/echo "BackspaceASCII"
/bin/echo -n "AB"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1

/bin/echo "Backspace漢字"
/bin/echo -n "漢字"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1
/bin/echo -ne "\x08"
sleep 1


/bin/echo "bell"
sleep 1
/bin/echo -ne "\007"
sleep 1

/bin/echo
/bin/echo -n "cursor off ==>"
/bin/echo -ne "\e[?25l"
sleep 2
/bin/echo

/bin/echo -n "cursor on ==>"
/bin/echo -ne "\e[?25h"
sleep 2
/bin/echo

/bin/echo -ne "\e[10;10Hx\x08"
sleep 1
/bin/echo -ne "\e[A"
sleep 1
/bin/echo -ne "\e[B"
sleep 1
/bin/echo -ne "\e[D"
sleep 1
/bin/echo -ne "\e[C"
sleep 1
/bin/echo -ne "\e[9A"
sleep 1
/bin/echo -ne "\e[9B"
sleep 1
/bin/echo -ne "\e[9D"
sleep 1
/bin/echo -ne "\e[9C"
sleep 1

/bin/echo -ne "\e[9A"
sleep 1
/bin/echo -ne "\e[A"
sleep 1
/bin/echo -ne "\e[A"
sleep 1
/bin/echo -ne "\e[A"
sleep 1
/bin/echo -ne "\e[9B"
sleep 1
/bin/echo -ne "\e[9B"
sleep 1
/bin/echo -ne "\e[B"
sleep 1
/bin/echo -ne "\e[B"
sleep 1
/bin/echo -ne "\e[B"
sleep 1

/bin/echo -ne "\e[5;10H\e[sx"
sleep 1
/bin/echo -ne "\e[10;20H\e[sx"
sleep 1
/bin/echo -ne "\e[15;30f\e[sx"
sleep 1
/bin/echo -ne "\e[20;40f\e[sx"
sleep 1
/bin/echo -ne "\e[uX"
sleep 1
/bin/echo -ne "\e[uX"
sleep 1
/bin/echo -ne "\e[uX"
sleep 1
/bin/echo -ne "\e[uX"
sleep 1

/bin/echo -ne "\x0d"
sleep 1
/bin/echo -ne "1234567890"
sleep 1
/bin/echo -ne "\eD"
sleep 1
/bin/echo -ne "1234567890"
sleep 1
/bin/echo -ne "\eM"
sleep 1
