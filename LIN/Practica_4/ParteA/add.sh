#!/bin/bash

for i in {1..10}; do
  echo "add $i" > /proc/modlist
  sleep 1
done

echo "add 11" > /proc/modlist