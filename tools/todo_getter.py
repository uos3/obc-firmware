"""
Author: Richard A 
Date: 2019-10-07
Maciej has provided TODOs in comments in the code. Maintaining the codebase is
definately neccisary for the completion of the project, and as such pinning
down incomplete sections.

This script goes through the source directory file by file and finds all
the "//TODO"s,  and produces a file containing 
1) The relative path of the file it is in
2) The line it is on
3) the contents of that line
"""
import os, sys, time

path_seperator = "/"
todo_search_string = "TODO"
# always include after the fact "/"
# only linux compatible "/" this is fixable
repo_root_folder = "obc-firmware" + path_seperator
source_folder = "src" + path_seperator

check_strings = [".c", ".h"]

repo_path = os.path.dirname(os.path.realpath(__file__)).split(repo_root_folder)[0] + repo_root_folder

class todo_obj:
    def __init__(self, path_string, file_line_number, todo_string):
        self.__dict__.update(locals())
        self.name = None


# need list of c and h files in each directory...
source_path = repo_path + source_folder
all_files_set = set()
for current_path, current_path_folders, current_path_files in os.walk(source_path):
    for file_name in current_path_files:
        for string in check_strings:
            if string in file_name:
                # triple nested for loop yay
                all_files_set.add(current_path + path_seperator + file_name)


todo_obj_list = []
for file_name in all_files_set:
    with open(file_name) as fp:
        # first check if there is a todo in the file, go line by line.
        # could use regex but won't get line number from it.
        text = fp.read()
        if todo_search_string in text:
            lines_list = text.split("\n")
            for index, line in enumerate(lines_list):
                if todo_search_string in line:
                    todo_text = todo_search_string + " " + line.split(todo_search_string)[1].lstrip()
                    local_name = file_name.split(repo_root_folder)[1]
                    todo_item = todo_obj(local_name, index, todo_text)
                    todo_obj_list.append(todo_item)

# do allocations
allocations_list =[
    ["Richard", 0],
    ["YZ", 0],
    ["Hubert", 0]
]

total_todos = len(todo_obj_list)
allocation_index = 0
current_file = todo_obj_list[0].path_string
mission_todos = []
for todo in todo_obj_list:
    if "src/mission/mission.c" in todo.path_string:
        mission_todos.append(todo)
        continue
    
    if todo.path_string != current_file:
        allocation_index += 1
        current_file = todo.path_string
        if allocation_index == 3:
            allocation_index = 0
    todo.name = allocations_list[allocation_index][0]
    allocations_list[allocation_index][1] += 1

target = total_todos // 3
total_leftover = len(mission_todos)
allocation_index = 0
for todo in mission_todos:
    if allocations_list[allocation_index][1] < target:
        pass
    elif allocation_index < 3:
        allocation_index += 1
    else:
        allocation_index = 0
    todo.name = allocations_list[allocation_index][0]
    allocations_list[allocation_index][1] += 1
# print(total_leftover)
print(allocations_list)




# generate markdown file with list in
md_list_template_string = "1. {todo_string}\n\n  PATH {path_string}\n\n  LINE NUMBER {file_line_number}\n\n  ALLOCATION {name}\n\n"
out_string = "\n# TODO list from codebase\n\n"
for todo in todo_obj_list:
    out_string+=md_list_template_string.format(**todo.__dict__)


out_file_name = time.strftime("todo_list_%Y%m%d.md")
with open(repo_path+"docs"+path_seperator+out_file_name, 'w') as fp:
    fp.write(out_string)
 
