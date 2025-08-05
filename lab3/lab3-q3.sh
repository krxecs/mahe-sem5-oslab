#!/usr/bin/env bash
D="$(echo "$2 * $2 - 4 * $1 * $3" | bc -l)"
c=1
if [ "$D" -eq 0 ]; then
  c=0
elif [ "$D" -lt 0 ]; then
  c=-1
fi

case "$D" in
0)
  echo "The root of this equation is $(echo "- $2 / (2 * $1)" | bc -l)"
  ;;
1)
  SD="$(echo "sqrt($D)" | bc -l)"
  S0="$(echo "(- $2 - $SD) / (2 * $1)" | bc -l)"
  S1="$(echo "(- $2 + $SD) / (2 * $1)" | bc -l)"
  echo "The roots of this equation are $S0 and $S1"
  ;;
*)
  echo "No real roots"
  ;;
esac

