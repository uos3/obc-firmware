'''
---- GENERATE DATAPOOL FUNCTIONS ----

This script starts in `DataPool_struct.h` and generates for the DataPool:
1. Functions that get and set values based on DP IDs
2. Functions that convert DP IDs into printable string symbols

This is done so that ground control may provide a list of DataPool items to
downlink as telemetry, in the form of 16-bit ID numbers. The ID numbers are
split into two parts, a block ID (0-127, 7 bits) and a block index (0-512, 9
bits). Blocks are assigned to modules, allowing easy expansion of an individual
module's DataPool without having to move other indexes or have members from
other modules in the middle of a module's DP IDs.

The IDs are specified as part of the documentation comment on a member, in the
form of:
```
/**
 * @brief Indicates whether or not the DataPool has been initialised.
 *
 * @dp 1
 */
bool INITIALISED;
```

Where the number following `@dp` is the index within the module's assigned
block space. This must be less than the total number of indexes assigned to the
module, for instance for a module with only one block assigned 512, for one
with 3 blocks this would be 1536.

DataPool parameters marked with `@dp_module ModuleName` indicate that the
member stores the DP parameters for the given module. The module name given
must match the one given in the block assignments listing in the documentation
string for the DataPool struct itself.
'''

import os
import re
import math
from datetime import datetime
from pprint import pprint
from pathlib import Path

def main():

    print('Starting DataPool code generation')

    # Get the root dir of OBC-Firmware
    root_dir = Path('/')
    for part in Path(os.getcwd()).parts:
        root_dir = root_dir.joinpath(part)
        if part == 'obc-firmware':
            break

    # Change into src
    src_dir = root_dir.joinpath('src')
    os.chdir(src_dir)

    # Load the root DP file
    with open('system/data_pool/DataPool_struct.h') as dp_struct_f:
        # Parse the file
        (datapool, version, includes) = parse_dp_struct(dp_struct_f.read())

    # Generate the new header and source files from the pool
    (source, header) = generate_files(datapool, version, includes)

    # Write the files out
    with open('system/data_pool/DataPool_generated.c', 'w+') as source_f:
        source_f.write(source)
    with open('system/data_pool/DataPool_generated.h', 'w+') as header_f:
        header_f.write(header)

    print('DataPool code generation complete')

def parse_dp_struct(dp_struct_text):
    '''
    Parse the DataPool_struct.h text into the datapool format for further
    processing.
    '''

    # Remove the file header comment and get the version of DataPool_struct.h
    (dp_struct_text, version) = get_version_and_remove_header(dp_struct_text)

    # Get list of all include files
    include_regex = re.compile('#include \"(.*?)\"')
    includes = [
        match.group(1) for match in include_regex.finditer(dp_struct_text)
    ]

    print('Will search the following included headers:')
    for include in includes:
        print(f'    {include}')

    # Extract all struct definitions from the includes
    included_structs = {}
    for include in includes:
        with open(include) as included_f:
            for name, text in get_structs(included_f.read()).items():
                included_structs[name] = text

    # Run the main regex over the file, this matches:
    # 1. A documentation comment.
    # 2. The immediately following line containing:
    #    a. the datatype
    #    b. the symbol
    #
    # The groups are therefore (match, docstring, datatype, symbol)
    regex = re.compile(
        r'/\*{2}(.*?)\*/\n\s*(.*?)\s+(.*?)\s*[;{]',
        flags=re.MULTILINE|re.S
    )
    matches = [match for match in regex.finditer(dp_struct_text)]

    # Get the match associated with the datapool, which has a symbol `struct
    # _DataPool`
    (dp_comment, dp_comment_idx) = [
        (match.group(1), idx)
        for (idx, match) in enumerate(matches)
        if match.group(3) == 'struct _DataPool'
    ][0]

    # Pop the DP comment off the matches list
    matches.pop(dp_comment_idx)

    print('Parsing block assignments')

    # Get the block assignments from this comment
    block_assignments = parse_block_assignments(dp_comment)

    print('Parsing DataPool')

    # Recursively parse the remaining matches
    datapool = parse_dp_matches(matches, included_structs, block_assignments)

    # Return the dict
    return (datapool, version, includes)

def get_version_and_remove_header(text):
    '''
    Remove the doxygen comment at the start of a file, and return the version
    number contained within that comment.
    '''

    regex = re.compile(r'@version\s+(.*?)$', flags=re.MULTILINE|re.S)
    version = regex.search(text)

    if version is not None:
        version = version.group(1)
    else:
        raise ValueError('DataPool_struct.h doc comment has no version number.')
    
    return (
        re.sub(r'/\*{2}(.*?)\*/', '', text, count=1, flags=re.MULTILINE|re.S),
        version
    )
    
def parse_block_assignments(text):
    '''
    Parse block assignments in the form `ModuleName: block0, block1`
    '''

    block_assignments = {}

    # Compile the regex
    regex = re.compile(r'\*\s+(.*?):\s+([\d,\s]+)\n')

    # Find all matches
    matches = [match for match in regex.finditer(text)]

    # Parse into the dict
    for match in matches:
        # The dict key is the module name and the value the block indexes,
        # which are parsed by splitting the index list on commas and parsing to
        # ints.
        # Parse the list of indexes
        idxs = []
        for idx in match.group(2).split(','):
            try:
                idxs.append(int(idx))
            except ValueError:
                pass

        # assign to block
        block_assignments[match.group(1)] = idxs

    return block_assignments

def parse_dp_matches(matches, included_structs, block_assignments):
    '''
    Parse all matches by descending through the members of DataPool to find
    included structs. The output format is a dictionary of symbols (like
    'DP.EVENTMANAGER.INITIALISED') to a dictionary containing block ID and
    index, datatype, and brief description.
    {
        'DP.EVENTMANAGER.INITIALISED': {
            'block_id': 1,
            'block_index': 1,
            'data_type': 'bool',
            'brief': 'Indicates whether or not the EventManager has been initialised.'
        }
    }
    '''

    # Preallocate the datapool
    datapool = {}

    # Compile regexes for matching different directives:
    # 1. @brief, the description of the parameter
    # 2. @dp, the index of the parameter in it's block
    # 3. @dp_module, indicates a module that should be descended into.
    brief_regex = re.compile(r'@brief\s(.*?)\s\*\s$', flags=re.MULTILINE|re.S)
    dp_regex = re.compile(r'@dp\s+(\d+)')
    dp_module_regex = re.compile(r'@dp_module\s+(.*?)$', flags=re.MULTILINE)

    # Iterate over all matches
    for match in matches:
        # First check if this match is a module to be descended into
        module = dp_module_regex.search(match.group(1))

        # If a module
        if module is not None:
            # Get the module struct from the included structs
            mod_struct = included_structs[match.group(2)]
            
            # Update the datapool with the parsed module struct
            datapool.update(parse_module_struct(
                block_assignments[module.group(1)],
                mod_struct,
                match.group(3)
            ))
        
        # If not a module run the brief and dp regexes too
        brief = brief_regex.search(match.group(1))
        dp = dp_regex.search(match.group(1))

        # If it has a DP tag
        if dp is not None:
            # If the brief is present
            if brief is not None:
                brief_text = brief.group(1)
                # Remove asterisks which start a line
                brief_text = re.sub(r'^\s+\*', '', brief_text, flags=re.MULTILINE)
                # Remove all new lines
                brief_text = re.sub(r'\n', '', brief_text)
                # Trim the text of leading/trailing whitespace
                brief_text = brief_text.strip()

            # Add the parameter to the data pool, noting that this is in the
            # DataPool module itself so the block ID is 0.
            datapool[f'DP.{match.group(3)}'] = {
                'block_id': 0,
                'block_index': int(dp.group(1)),
                'data_type': match.group(2),
                'brief': brief_text
            }

    return datapool

def parse_module_struct(block_ids, text, symbol):
    '''
    Parse the given text as a module struct, with the given symbol name and
    block ID.
    '''

    mod_dp = {}

    # Run the standard regex over the module struct
    regex = re.compile(
        r'/\*{2}(.*?)\*/\n\s*(.*?)\s+(.*?)\s*[;{]',
        flags=re.MULTILINE|re.S
    )
    matches = [match for match in regex.finditer(text)]

    # For all matches run the brief and index regexes
    brief_regex = re.compile(r'@brief\s(.*?)\s\*\s$', flags=re.MULTILINE|re.S)
    dp_regex = re.compile(r'@dp\s+(\d+)')

    for match in matches:
        brief = brief_regex.search(match.group(1))
        dp = dp_regex.search(match.group(1))

        # If it has a DP tag
        if dp is not None:
            # If the brief is present
            if brief is not None:
                brief_text = brief.group(1)
                # Remove asterisks which start a line
                brief_text = re.sub(r'^\s+\*', '', brief_text, flags=re.MULTILINE)
                # Remove all new lines
                brief_text = re.sub(r'\n', '', brief_text)
                # Trim the text of leading/trailing whitespace
                brief_text = brief_text.strip()

            # DP index
            dp_idx = int(dp.group(1))

            # Calculate which block the module is assigned to this member fits
            # in. For example if the index is 512->1023 it would go in the 2nd
            # index.
            block_id_idx = math.floor(dp_idx/512)

            # If the index is greater than the length of the assigned blocks
            # then raise an error
            if block_id_idx > len(block_ids):
                raise ValueError(
                    f'{symbol} is assigned {len(block_ids)} blocks but has more than {len(block_ids)*512} members'
                )

            # Add the parameter to the data pool, noting that this is in the
            # DataPool module itself so the block ID is 0.
            mod_dp[f'DP.{symbol}.{match.group(3)}'] = {
                'block_id': block_ids[block_id_idx],
                'block_index': int(dp.group(1)),
                'data_type': match.group(2),
                'brief': brief_text
            }

    return mod_dp

def get_structs(text):
    '''
    Match all structs (typedef struct _StructName {...} StructName;) in some
    text.

    Note: This regex doesn't handle multiple braces in the struct definition,
    so {{}} will break. This isn't expected to be a problem, but it could be at
    some point?
    '''

    structs = {}

    # Compile and match regex
    regex = re.compile(
        r'typedef\s+struct\s+.*?\s+{(.*?)}\s+(.*?)\s*;', 
        flags=re.MULTILINE|re.S
    )
    matches = [match for match in regex.finditer(text)]

    # Gather the structs in a dict, where key is the struct name and value is
    # the struct text.
    for match in matches:
        structs[match.group(2)] = match.group(1)

    return structs

def generate_files(datapool, version, includes):
    '''
    Generate the source and header files from the datapool definition
    '''

    # Find all datatypes
    data_type_map = {}
    for member in datapool.values():
        if member['data_type'] not in data_type_map.keys():
            data_type_map[member['data_type']] = \
                f'DATAPOOL_DATATYPE_{member["data_type"].upper()}'

    newline = '\n'

    # Write the header part of the source and header files
    source = \
f'''
/**
 * @ingroup data_pool
 * 
 * @file DataPool_generated.c
 * @author Generated by DataPool_generate.py
 * @brief Generated code to assist with DataPool usage.
 * 
 * This file was generated from DataPool_struct.h by DataPool_generate.py.
 * 
 * @version Generated from DataPool_struct.h version {version}
 * @date {datetime.today().strftime('%Y-%m-%d')}
 * 
 * @copyright Copyright (c) UoS3 2020
 */

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stddef.h>

/* Internal includes */
#include "system/data_pool/DataPool_public.h"
#include "system/data_pool/DataPool_generated.h"

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

bool DataPool_get(
    DataPool_Id id_in,
    void **pp_data_out,
    DataPool_DataType *p_data_type_out,
    size_t *p_data_size_out
) {{

    switch (id_in) {{
    
    {newline.join([
        get_parameter_code(symbol, dp_value, data_type_map) 
        for symbol, dp_value 
        in datapool.items()
    ])}
    
    default:
        return false;
    }}
}}

bool DataPool_get_symbol_str(
    DataPool_Id id_in, 
    char **pp_symbol_str_out
) {{

    switch (id_in) {{
    
    {newline.join([
        get_symbol_str_code(symbol, dp_value) 
        for symbol, dp_value 
        in datapool.items()
    ])}

    default:
        return false;
    }}
}}
'''
    header = \
f'''
/**
 * @ingroup data_pool
 * 
 * @file DataPool_generated.h
 * @author Generated by DataPool_generate.py
 * @brief Generated code to assist with DataPool usage.
 * 
 * This file was generated from DataPool_struct.h by DataPool_generate.py.
 * 
 * @version Generated from DataPool_struct.h version {version}
 * @date {datetime.today().strftime('%Y-%m-%d')}
 * 
 * @copyright Copyright (c) UoS3 2020
 */

#ifndef H_DATAPOOL_GENERATED_H
#define H_DATAPOOL_GENERATED_H

/* -------------------------------------------------------------------------   
 * INCLUDES
 * ------------------------------------------------------------------------- */

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Internal includes */
{newline.join([f'#include "{include}"' for include in includes])}

/* -------------------------------------------------------------------------
 * TYPES
 * ------------------------------------------------------------------------- */

/**
 * @brief Type representing an ID to access the DataPool.
 */
typedef uint16_t DataPool_Id;

/* -------------------------------------------------------------------------   
 * ENUMS
 * ------------------------------------------------------------------------- */

/**
 * @brief Types of data that are contained in the DataPool.
 */
typedef enum _DataPool_DataType {{
    {newline.join(data_type_map.values())}
}} DataPool_DataType;

/* -------------------------------------------------------------------------   
 * FUNCTIONS
 * ------------------------------------------------------------------------- */

/**
 * @brief Get a pointer to the DataPool parameter associated with the given ID,
 * as well as the type of the data and the number of bytes the data occupies.
 * 
 * @param id_in The ID of the DataPool parameter.
 * @param p_data_out Output pointer to a pointer to the parameter.
 * @param p_data_type_out Output pointer to the data type of the parameter.
 * @param p_data_size_out Output pointer to the size (in bytes) of the 
 *        parameter.
 * @return bool True if successful, false if invalid id.
 */
bool DataPool_get(
    DataPool_Id id_in,
    void **pp_data_out,
    DataPool_DataType *p_data_type_out,
    size_t *p_data_size_out
);

/**
 * @brief Fill a buffer with the symbol name of the DataPool parameter with the
 * given ID.
 *
 * The caller is responsible for freeing the memory allocated for the symbol
 * string.
 *
 * @param id_in The ID of the DataPool parameter.
 * @param pp_symbol_str_out Pointer to the buffer to store the string in.
 * @return bool True if successful, false if the ID is invalid.
 */
bool DataPool_get_symbol_str(
    DataPool_Id id_in, 
    char **pp_symbol_str_out
);

#endif /* H_DATAPOOL_GENERATED_H */
'''

    return (source, header)

def get_parameter_code(symbol, dp_value, data_type_map):
    '''
    Return a string for the given symbol that returns the case statement for 
    get_symbol_str.
    '''
    # Calculate the index, shifting the block id 9 bits to the left.
    idx = dp_value['block_id'] << 9 | dp_value['block_index']
    return \
f'''
    /* {symbol} */
    case 0x{idx:04x}:
        *pp_data_out = &{symbol};
        *p_data_type_out = {data_type_map[dp_value["data_type"]]};
        *p_data_size_out = sizeof({dp_value["data_type"]});
        return true;
'''

def get_symbol_str_code(symbol, dp_value):
    '''
    Return a string for the given symbol that returns the case statement for 
    get_symbol_str.
    '''
    # Calculate the index, shifting the block id 9 bits to the left.
    idx = dp_value['block_id'] << 9 | dp_value['block_index']
    return \
f'''
    /* {symbol} */
    case 0x{idx:04x}:
        *pp_symbol_str_out = strdup("{symbol}");
        return true;
'''

if __name__ == '__main__':
    main()