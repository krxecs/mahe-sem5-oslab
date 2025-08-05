#!/usr/bin/env bash
if [ "$(expr "$1" '%' 2 )" -eq 0 ]; then
  echo -n "Even"
else
  echo -n "Odd"
fi
echo " number"

