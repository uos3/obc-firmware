'''
Test of DataPool generation using python and an excel definition file in the
sharepoint.
'''

from office365.runtime.auth.authentication_context import AuthenticationContext
from office365.sharepoint.client_context import ClientContext
from office365.sharepoint.files.file import File
import xlrd
import getpass
import sys
from pathlib import Path

newline = '\n'

# Outdir is passed in as an argument
outdir = Path(str(sys.argv[1]))

def struct_member(symbol, data_type, brief, detailed):
    return f'''
    /**
     * @brief {brief}
     *
     * {detailed}
     */
    {data_type} {symbol};

    '''

def decl_get_func(data_type):
    return f'''
/**
 * @brief Get the value in the DataPool associated with the given ID.
 *
 * @param id_in The ID of the DataPool member.
 * @param p_value_out Pointer to the location to store the retrieved value.
 * @return true The retrieval was successful.
 * @return false Invalid ID.
 */
bool DataPool_get_{data_type}(DataPoolId id_in, {data_type} *p_value_out);
'''

def impl_get_func(data_type, symbols, ids):
    return f'''
bool DataPool_get_{data_type}(DataPoolId id_in, {data_type} *p_value_out) {{
    switch (id_in) {{
    {newline.join([get_value(symbols[i], ids[i]) for i in range(len(ids))])}
    default:
        return false;
    }}

    return true;
}}
'''

def get_value(symbol, id):
    return f'''
    /* {symbol} */
    case {id}:
        *p_value_out = DP.{symbol};
        break;
    '''

def decl_set_func(data_type):
    return f'''
/**
 * @brief Set the value in the DataPool associated with the given ID.
 *
 * @param id_in The ID of the DataPool member.
 * @param value_in The value to store in the DataPool.
 * @return true The operation completed successfully.
 * @return false Invalid ID.
 */
bool DataPool_set_{data_type}(DataPoolId id_in, {data_type} value_in);
'''

def impl_set_func(data_type, symbols, ids):
    return f'''
bool DataPool_set_{data_type}(DataPoolId id_in, {data_type} value_in) {{
    switch (id_in) {{
    {newline.join([set_value(symbols[i], ids[i]) for i in range(len(ids))])}
    default:
        return false;
    }}

    return true;
}}
'''

def set_value(symbol, id):
    return f'''
    case {id}:
        DP.{symbol} = value_in;
        break;
    '''

def symbol_str_case(symbol, id):
    return f'''
    /* {symbol} */
    case {hash_djb2(symbol)}:
        return {id};
    '''

def hash_djb2(s):                                                                                                                                
    hash = 5381
    for x in s:
        hash = (( hash << 5) + hash) + ord(x)
    return hash & 0xFFFFFFFF


# # Get username and password
username = input('SharePoint Username: ')
password = getpass.getpass('SharePoint Password: ')

url = 'https://sotonac.sharepoint.com/:x:/r/teams/UniversityofSouthamptonSmallSatelliteUoS3'

ctx_auth = AuthenticationContext(url)
ctx_auth.acquire_token_for_user(username, password)
ctx = ClientContext(url, ctx_auth)

response = File.open_binary(ctx, "/teams/UniversityofSouthamptonSmallSatelliteUoS3/Shared Documents/Software/SystemSoftwareSpecification/DataPool/UoS3_DataPoolDefinition_Working.xlsx")

with open("./UoS3_DataPoolDefinition_Working.xlsx", "wb") as local_file:
    local_file.write(response.content)

workbook = xlrd.open_workbook('./UoS3_DataPoolDefinition_Working.xlsx')
sheet = workbook.sheet_by_index(0)

# Populate the DP definition table
datapool_definition = {}

for col_idx in range(1, sheet.ncols):
    field = sheet.row_values(3)[col_idx]

    if field == 'ID':
        datapool_definition[field] = [int(x) for x in sheet.col_values(col_idx)[4:]]
    else:
        datapool_definition[field] = sheet.col_values(col_idx)[4:]

# Close the workbook
workbook.release_resources()

struct_members = []
symbol_str_cases = []
tables_by_datatype = {}
for i in range(len(datapool_definition['ID'])):
    struct_members.append(struct_member(
        datapool_definition['Symbol'][i], 
        datapool_definition['Data Type'][i], 
        datapool_definition['Brief'][i], 
        datapool_definition['Detailed'][i], 
    ))

    symbol_str_cases.append(symbol_str_case(
        datapool_definition['Symbol'][i],
        datapool_definition['ID'][i]
    ))

    if datapool_definition['Data Type'][i] not in tables_by_datatype.keys():
        tables_by_datatype[datapool_definition['Data Type'][i]] = {}
        tables_by_datatype[datapool_definition['Data Type'][i]]['symbols'] = []
        tables_by_datatype[datapool_definition['Data Type'][i]]['ids'] = []

    tables_by_datatype[datapool_definition['Data Type'][i]]['symbols'].append(
        datapool_definition['Symbol'][i]
    )
    tables_by_datatype[datapool_definition['Data Type'][i]]['ids'].append(
        datapool_definition['ID'][i]
    )

get_func_decls = []
get_func_impls = []
set_func_decls = []
set_func_impls = []
for data_type, data in tables_by_datatype.items():
    get_func_decls.append(decl_get_func(data_type))
    set_func_decls.append(decl_set_func(data_type))
    
    get_func_impls.append(impl_get_func(data_type, data['symbols'], data['ids']))
    set_func_impls.append(impl_set_func(data_type, data['symbols'], data['ids']))

# Build the header file
header_file = f'''\
/**
 * TODO
 */
#ifndef H_DATAPOOL_STRUCT_H
#define H_DATAPOOL_STRUCT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _DataPool {{
{newline.join(struct_members)}
}} DataPool;

typedef uint16_t DataPoolId;

DataPoolId DataPool_get_id_from_symbol(char *p_symbol_in);

{newline.join(get_func_decls)}

{newline.join(set_func_decls)}

#endif
'''

source_file = f'''\
/**
 * TODO
 */

#include <stdint.h>
#include <stdbool.h>

#include "system/data_pool/DataPool_public.h"

unsigned long hash_djb2(unsigned char *str)
{{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash & 0xFFFFFFFF;
}}

DataPoolId DataPool_get_id_from_symbol(char *p_symbol_in) {{
    printf("Symbol hash: %ld\\n", hash_djb2(p_symbol_in));
    switch (hash_djb2(p_symbol_in)) {{
    {newline.join(symbol_str_cases)}
    default:
        return 0;
    }}
}}

{newline.join(get_func_impls)}

{newline.join(set_func_impls)}
'''

print('Writing header file DataPool_generated.h')
with open(outdir.joinpath('DataPool_generated.h'), 'w+') as f:
    f.write(header_file)

print('Writing source file DataPool_generated.c')
with open(outdir.joinpath('DataPool_generated.c'), 'w+') as f:
    f.write(source_file)
