#!/bin/bash

# Check if the correct number of arguments is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename.c>"
    exit 1
fi

# Compile the file using gcc
gcc -Wall -o "${1%.c}" "$1" 2> output.txt

# Count the number of warnings and errors
warnings=$(grep -c "warning:" output.txt)
errors=$(grep -c "error:" output.txt)

# Display the results
echo "Compilation results for $1:"
echo "  Warnings: $warnings"
echo "  Errors: $errors"

# Clean up
rm -f output.txt "${1%.c}"
