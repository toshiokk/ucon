#!/bin/bash

echo "background=49,40~47,100~107, foreground=39,30~37,90~97"
for back in 49 {40..47}; do
  /bin/echo -ne "\e[0m0${back}:"
  for fore in 39 {30..37} {90..97}; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
for back in {100..107}; do
  /bin/echo -ne "\e[0m${back}:"
  for fore in 39 {30..37} {90..97}; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "highlight=off, reverse=off"
for back in 49 {40..47}; do
  /bin/echo -ne "\e[0m${back}:"
  for fore in {30..37} 39; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "highlight=on, reverse=off"
for back in 49 {40..47}; do
  /bin/echo -ne "\e[0m${back}:"
  /bin/echo -ne "\e[1;27m"
  for fore in {30..37} 39; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "highlight=off, reverse=on"
for back in 49 {40..47}; do
  /bin/echo -ne "\e[0m${back}:"
  /bin/echo -ne "\e[7m"
  for fore in {30..37} 39; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "highlight=on, reverse=on"
for back in 49 {40..47}; do
  /bin/echo -ne "\e[0m${back}:"
  /bin/echo -ne "\e[1;7m"
  for fore in {30..37} 39; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "background=normal, foreground=bright"
for back in 49 {40..47}; do
  /bin/echo -ne "\e[0m${back}:"
  for fore in {90..97}; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "background=bright, foreground=normal"
for back in {100..107}; do
  /bin/echo -ne "\e[0m${back}:"
  for fore in {30..37} 39; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "background=bright, foreground=bright"
for back in {100..107}; do
  /bin/echo -ne "\e[0m${back}:"
  for fore in {90..97}; do
    /bin/echo -ne "\e[${fore};${back}m${fore}"
  done
  /bin/echo -e "\e[0m"
done
sleep 1

echo "----------------------------------------------------"
