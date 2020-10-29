'''
---- DATAPOOL GENERATION ----

The DataPool structure is defined in the DataPoolDefinition Excel spreadsheet,
for two main reasons:
    1. Easy control of ID numbers for DataPool members.
    2. Simplification of the additional code required to use DataPool IDs to
       get and set DataPool members.

DataPool IDs (`DataPoolId` in C) each identify a single member of the pool.
They are used by ground when specifying which DP parameters should be included
in TM packets, used for parameter monitoring, and for FDIR event reporting.

This script downloads the latest (or specific) version of the
DataPoolDefinition spreadsheet from the project SharePoint, located at
/Software/SystemSoftwareSpecification/DataPool/UoS3_DataPoolDefinition_vX.xlsd.
You will be prompted to enter a username and password to authenticate with the
project SharePoint. This session will be saved for future use.
'''

from pathlib import Path
import argparse
import sharepy
import xlrd

def main():
    '''
    Run the main script.
    '''

    # Get the command line args
    paths = parse_args()

    # Create directories if don't exist
    if not paths['wbpath'].parent.exists():
        paths['wbpath'].parent.mkdir()
    if not paths['out'].exists():
        paths['out'].mkdir()

    # Download the excel workbook
    download_definition(paths['wbpath'], paths['cookie'])

    # Parse the workbook into the definition structure
    datapool_definition = parse_workbook(paths['wbpath'])

    # Generate the output files
    generate_output(datapool_definition, paths['out'])

def parse_args():
    '''
    Parse command line arguments
    '''

    # Create argparser
    parser = argparse.ArgumentParser(description='Generate DataPool structure and convenience functions.')
    parser.add_argument(
        'wbpath',
        help='Path to save the DataPool definition workbook to',
        nargs='?',
        default='./tmp/DataPoolDefinition.xlsx'
    )
    parser.add_argument(
        'outdir',
        help='Path to the directory to output the generated header and source files.',
        nargs='?',
        default='.'
    )
    parser.add_argument(
        'cookiedir',
        help='Path to the file to store the session cookie in.',
        nargs='?',
        default='./sp-session.pkl'
    )

    # Parse the arguments
    args = parser.parse_args()

    # Convert to Paths
    paths = {}
    paths['wbpath'] = Path(args.wbpath)
    paths['out'] = Path(args.outdir)
    paths['cookie'] = Path(args.cookiedir)

    return paths

def download_definition(wbpath, cookiepath):
    '''
    Download the DataPoolDefinition workbook from the SharePoint.
    '''

    # Attempt to load the previous session
    try:
        session = sharepy.load(cookiepath)

    # If no session was loaded start a new one
    except FileNotFoundError:
        session = sharepy.connect('https://sotonac.sharepoint.com')

    # Save the session
    session.save()

    # Get the workbook file
    response = session.getfile(
        'https://sotonac.sharepoint.com/:x:/r/teams/UniversityofSouthamptonSmallSatelliteUoS3/Shared%20Documents/Software/SystemSoftwareSpecification/DataPool/UoS3_DataPoolDefinition_Working.xlsx',
        filename=wbpath
    )
    
    if response.status_code == 200:
        print('DataPool definition downloaded')
    else:
        print(f'DataPool definition download failed: {response.content}')
        raise RuntimeError()
    
def parse_workbook(wbpath):
    '''
    Parse the definition workbook into an intermediary object format
    '''
    # Preallocate the DP definition dict
    dp_def = {}

    # Load the workbook
    wb = xlrd.open_workbook(wbpath)

    # Release all the resources
    wb.release_resources()

    # Return the definition
    return dp_def

def generate_output(dp_def, outdir):
    '''
    Generate the output header and source files from the definition and put
    them in the given output directory.
    '''
    pass

if __name__ == '__main__':
    main()