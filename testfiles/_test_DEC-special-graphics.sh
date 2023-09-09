#!/bin/bash

echo "DEC special graphics"
/bin/echo -e "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\`abcdefghijklmnopqrstuvwxyz{|}~"
/bin/echo -ne "\e(0"
/bin/echo -e "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\`abcdefghijklmnopqrstuvwxyz{|}~"
/bin/echo -ne "\e(B"

echo "----------------------------------------------------"
