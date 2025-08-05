#!/usr/bin/env bash
find . -maxdepth 1 -path "$1" -exec cp -r {} "$2" \;
