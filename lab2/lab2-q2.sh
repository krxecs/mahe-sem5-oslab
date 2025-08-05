#!/usr/bin/env bash
find "$1" -maxdepth 1 -name "$2" -exec echo {} \;
