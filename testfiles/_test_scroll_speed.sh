/bin/echo -ne "\e[1;1H"
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
sleep 2
beginsec=`date "+%s"`
cat all-utf8.txt
endsec=`date "+%s"`
sec=`expr $endsec - $beginsec`
echo "$sec seconds"
sleep 1

/bin/echo -ne "\e[r"
/bin/echo "scroll area cleared"
sleep 2
beginsec=`date "+%s"`
cat all-utf8.txt
endsec=`date "+%s"`
sec=`expr $endsec - $beginsec`
echo "$sec seconds"
sleep 1
