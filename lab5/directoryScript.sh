#!/bin/bash

# Check if a directory name was provided as argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 directory_name"
    exit 1
fi

dir_name=$1

# Check if the provided argument is a directory
if [ ! -d "$dir_name" ]; then
    echo "$dir_name is not a directory."
    exit 1
fi

# Create the file with the directory name
file_name="${dir_name}/${dir_name}_file.txt"
touch "$file_name"
echo "Created file: $file_name"
