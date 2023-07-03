pause()
{
#  read key
  sleep 1
}
/bin/echo "clear screen"
pause
/bin/echo -ne "\ec"
pause

/bin/echo -ne "\ec"
cat _杜子春.txt
/bin/echo -ne "\e[10;10H"
/bin/echo -ne "\e[7m clear current to bottom line \e[0m"
pause
/bin/echo -ne "\e[0K"
pause

/bin/echo -ne "\ec"
cat _杜子春.txt
/bin/echo -ne "\e[10;10H"
/bin/echo -ne "\e[7m clear top to current line \e[0m"
pause
/bin/echo -ne "\e[1K"
pause

/bin/echo -ne "\ec"
cat _杜子春.txt
/bin/echo -ne "\e[10;10H"
/bin/echo -ne "\e[7m clear all lines \e[0m"
pause
/bin/echo -ne "\e[2K"
pause

/bin/echo -ne "\ec"
cat _杜子春.txt
/bin/echo -ne "\e[10;10H"
/bin/echo -ne "\e[7m clear current to bottom line \e[0m"
pause
/bin/echo -ne "\e[0J"
pause

/bin/echo -ne "\ec"
cat _杜子春.txt
/bin/echo -ne "\e[10;10H"
/bin/echo -ne "\e[7m clear top to current line \e[0m"
pause
/bin/echo -ne "\e[1J"
pause

/bin/echo -ne "\ec"
cat _杜子春.txt
/bin/echo -ne "\e[10;10H"
/bin/echo -ne "\e[7m clear all lines \e[0m"
pause
/bin/echo -ne "\e[2J"
pause

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m insert 4 columns \e[0m"
pause
/bin/echo -ne "\e[4@"
pause

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m insert 5 columns \e[0m"
pause
/bin/echo -ne "\e[5@"
pause

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m delete 4 columns \e[0m"
pause
/bin/echo -ne "\e[4P"
pause

/bin/echo -ne "\ec"
cat _GPL.txt
/bin/echo -ne "\e[5;10H"
/bin/echo -ne "\e[7m delete 5 columns \e[0m"
pause
/bin/echo -ne "\e[5P"
pause
