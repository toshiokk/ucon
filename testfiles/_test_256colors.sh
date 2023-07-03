#!/bin/bash
for reverse in {0..1} ; do
for back_fore in {0..1} ; do
  if [ $back_fore = 0 ]; then
    echo "changing foreground color -- "
  else
    echo "changing background color -- "
  fi
  if [ $reverse = 0 ]; then
    echo "reverse off"
  else
    echo "reverse on"
  fi
  for i in {0..255} ; do
    if [ $reverse = 0 ]; then
      printf "\x1b[0m"
    else
      printf "\x1b[7m"
    fi
    printf "\x1b[37m\x1b[47m"
    if [ $back_fore = 0 ]; then
      printf "\x1b[38;5;%sm%03d\x1b[0m " "$i" "$i"
    else
      printf "\x1b[48;5;%sm%03d\x1b[0m " "$i" "$i"
    fi
    if (( i < 16 )); then
      if (( i % 16 == 15 )); then
        printf "\n";
      fi
    elif (( (i < 232) )); then
      if (( (i - 16) % 18 == 17 )); then
        printf "\n";
      elif (( (i - 16) % 6 == 5 )); then
        printf " ";
      fi
    elif (( (i - 232) % 8 == 7 )); then
      printf "\n";
    else
      printf "";
    fi
  done
  sleep 2
done
done
echo "----------------------------------------------------"
