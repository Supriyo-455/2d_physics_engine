#!/usr/bin/env bash

export PATH="$PWD:$PATH"
echo "Shell initialized for 2D Physics Engine."

# Capture all .sh files into an array
SHELL_SCRIPTS=("$PWD/misc"/*.sh)

# Iterate safely over the array
for file in "${SHELL_SCRIPTS[@]}"
do
    # Make all files executable
	if [ -f "$file" ]; then
        chmod +x $file
    fi
done