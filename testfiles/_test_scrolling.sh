/bin/echo -ne "\e[r"
/bin/echo "scroll area cleared"
sleep 1
/bin/echo -ne "\eD"
sleep 1
/bin/echo -ne "\eM"
sleep 1
/bin/echo -ne "\eM"
sleep 1

/bin/echo "1"
/bin/echo "2"
/bin/echo "3"
/bin/echo "4"
/bin/echo "5"
/bin/echo "6"
/bin/echo "7"
/bin/echo "8"
/bin/echo "9"
/bin/echo "10"
/bin/echo "11"
/bin/echo "12"

/bin/echo -ne "\e[6;10r"
/bin/echo "scroll area set (6,10)"
/bin/echo -ne "\e[1;1H"
sleep 1
beginsec=`date "+%s"`
cat all-utf8.txt
endsec=`date "+%s"`
sec=`expr $endsec - $beginsec`
echo "$sec seconds"
sleep 1

/bin/echo -ne "\e[r"
/bin/echo "scroll area cleared"
sleep 1
beginsec=`date "+%s"`
cat all-utf8.txt
endsec=`date "+%s"`
sec=`expr $endsec - $beginsec`
echo "$sec seconds"
sleep 1

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m scroll down 3 lines from cursor line \e[0m"
sleep 1
/bin/echo -ne "\e[3L"
sleep 1

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m scroll up 3 lines from cursor line \e[0m"
sleep 1
/bin/echo -ne "\e[3M"
sleep 1

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m scroll down whole screen 3 lines \e[0m"
sleep 1
/bin/echo -ne "\e[3S"
sleep 1

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m scroll up whole screen 3 lines \e[0m"
sleep 1
/bin/echo -ne "\e[3T"
sleep 1
