#!/bin/bash

for i in {1..5}; do
  echo "remove $i" > /proc/modlist
  sleep 1
done

echo "cleanup" > /proc/modlist