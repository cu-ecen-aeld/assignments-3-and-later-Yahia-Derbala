#!/bin/sh

# Check if the correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <directory> <search_string>"
    exit 1
fi

# Assign the provided arguments to variables
filesdir="$1"
searchstr="$2"

# Check if filesdir is a directory
if [ ! -d "$filesdir" ]; then
    echo "Error: $filesdir is not a valid directory."
    exit 1
fi

# Initialize counters for the number of files and matching lines
file_count=0
matching_lines_count=0

# Recursive function to search for matching lines in files
search_files() {
    local directory="$1"
    for file in "$directory"/*; do
        if [ -d "$file" ]; then
            search_files "$file"
        elif [ -f "$file" ]; then
            file_count=$((file_count + 1))
            matching_lines=$(grep -c "$searchstr" "$file")
            matching_lines_count=$((matching_lines_count + matching_lines))
        fi
    done
}

# Call the search_files function with the provided directory
search_files "$filesdir"

# Print the results
echo "The number of files are $file_count and the number of matching lines are $matching_lines_count"

