#!/usr/bin/env bash
r=1
for i in $(seq 1 "$1"); do
  r="$(expr "$r" \* "$i")"
done
echo "$r"

