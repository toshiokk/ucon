#!/bin/bash
for reverse in {0..1} ; do
for back_fore in {0..1} ; do
  if [ $back_fore = 0 ]; then
    echo -n "changing foreground color -- "
  else
    echo -n "changing background color -- "
  fi
  if [ $reverse = 0 ]; then
    echo "reverse off"
  else
    echo "reverse on"
  fi
  for r in 0 51 102 153 204 255 ; do
  for g in 0 51 102 153 204 255 ; do
  for b in 0 51 102 153 204 255 ; do
    if [ $reverse = 0 ]; then
      printf "\x1b[0m"
    else
      printf "\x1b[7m"
    fi
    printf "\x1b[37m\x1b[47m"
    if [ $back_fore = 0 ]; then
      printf "\x1b[38;2;%s;%s;%sm%02x%02x%02x \x1b[0m" "$r" "$g" "$b" "$r" "$g" "$b"
    else
      printf "\x1b[48;2;%s;%s;%sm%02x%02x%02x \x1b[0m" "$r" "$g" "$b" "$r" "$g" "$b"
    fi
  done
    printf "\n";
  done
  sleep 1
  done
  sleep 2
done
done
echo "----------------------------------------------------"
