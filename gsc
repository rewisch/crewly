#!/usr/bin/env bash

files=$(find . -type f \( -name "*.c" -o -name "*.h" \))

if [ -z "$files" ]; then
    echo "No .c or .h files found."
    exit 1
fi

{
    for f in $files; do
        echo "========== FILE: $f =========="
        cat "$f"
        echo
        echo
    done
} | wl-copy

echo "Copied all .c/.h files recursively into clipboard."
