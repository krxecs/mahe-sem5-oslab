#!/usr/bin/env bash

OLD_IFS="$IFS"
IFS=":"
find $PATH -type f -perm /u+x,g+x,o+x
IFS="$OLD_IFS"

