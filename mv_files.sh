#! /usr/bin/env bash

source_file=$1
source_dir="src/board/"
source_dir_c="src/board/uos3-proto/"
dest_dir="src/driver/"

mv $source_dir$source_file.h $dest_dir$source_file.h
mv $source_dir_c$source_file.c $dest_dir$source_file.c
