#!/usr/bin/env bash

set -e

PROD=false

while getopts "p" flag; do
  case "${flag}" in
    p) PROD=true ;;
  esac
done

if [ "$PROD" = true ]; then
  echo "Building PRODUCTION version..."
  gcc ./src/main.c \
    -o ./dst/crewly \
    -O2 \
    -flto \
    -s \
    -DNDEBUG
else
  echo "Building DEVELOPMENT version..."
  gcc ./src/main.c \
    -o ./dst/crewly \
    -g \
    -Wall \
    -Wextra \
    -DDEBUG
fi
