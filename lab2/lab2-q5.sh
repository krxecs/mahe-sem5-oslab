#!/usr/bin/env bash
sort --merge "$1" "$2" | uniq
