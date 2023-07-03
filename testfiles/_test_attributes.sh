#!/bin/bash
for x in {0..8} {21..28}; do
  /bin/echo -ne "${x}"
  for fa in {30..37}; do
    f=`expr $fa % 8`
    for ba in {40..47}; do
      b=`expr $ba % 8`
      /bin/echo -ne "\e[$x;$fa;${ba}m${f}${b}\e[0m"
    done
    echo -n " "
  done
  echo
done
echo ""
