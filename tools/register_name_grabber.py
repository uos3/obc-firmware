#! /usr/bin/python3
# Python script to create a list of strings against addresses for C pre-processor defines.

target_filename = "../src/component/cc112x_spi.h"
target_file_startline = 54
target_file_stopline = 273
longest_namelength = 30

with open(target_filename, 'r') as file:
	lines = file.readlines()

# generate relevant lines
relevant_lines = []
for line in lines[target_file_startline: target_file_stopline +1]:
	if "#define " in line:
		truncated_line = line.split("#define ")[-1][:-1]
		relevant_lines.append(truncated_line)

# split relevant lines into strings and values
name_value_pairs = []
for line in relevant_lines:
	if "/*" in line:
		line = line.split("/*")[0]
	linesplit  = line.split(" ")
	valid = []
	for entry in linesplit:
		if len(entry) > 0:
			valid.append(entry)
	name = valid[0]
	value = valid[-1]
	name_value_pairs.append([name, value])

# format name/value pairs into a C friendly format
c_namelist= f"char register_names[][{longest_namelength}] = {{\n"
c_namelist_line = '\t"{}",\n'
c_valuelist = "uint32_t register_addresses[] = {\n"
c_valuelist_line = "\t{},\n"
c_end = "\t};\n"

for name, value in name_value_pairs:
	c_namelist+=c_namelist_line.format(name)
	c_valuelist+=c_valuelist_line.format(value)

c_namelist += c_end
c_valuelist += c_end

# write to file

with open("c_output.txt", 'w') as file:
	file.write(c_namelist)
	file.write(c_valuelist)

