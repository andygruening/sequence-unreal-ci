#!/bin/sh

if [ -f /ready/flag ]; then
  echo "Ready"
  exit 0
else
  echo "Not ready"
  exit 1
fi