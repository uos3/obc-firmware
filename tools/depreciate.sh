#!/usr/bin/env bash

# tool to deprecite a passed file name

software_root_dir=""
depfname="depreciated"

if [ $PWD == *"tools"* ]
	then
	echo "in tools dir. changing path."
	software_root_dir="../"
fi

filepath=$1

# split string into array
IFS="/"
read -ra path_array <<< "$filepath"
IFS=" "

source_path=${software_root_dir}${filepath}
filename=${path_array[-1]}

# remove last element fo the array
unset path_array[${#path_array[@]}-1]

# make file directory string
file_directory=""
for folder in "${path_array[@]}"
do
	file_directory+="$folder/"
	# echo $file_directory
done

# 
dest_path="${software_root_dir}${file_directory}${depfname}/"
# echo $source_path
# echo $dest_path

git mv $source_path $dest_path
git commit -m "depreciated $source_path"
# echo "done"
