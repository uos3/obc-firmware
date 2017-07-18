#!/usr/bin/env bash

source_dir="$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)"
cd $source_dir

opt_quiet=false

COLOUR_GREEN='\033[0;32m'
COLOUR_YELLOW='\033[1;33m'
COLOUR_PURPLE='\033[0;35m'
COLOUR_RED='\033[0;31m'
COLOUR_OFF='\033[0;00m'
CLEAR_LINE='\033[2K'

_ERROR_="$COLOUR_RED[ERROR]$COLOUR_OFF"
_INFO_="$COLOUR_PURPLE[INFO]$COLOUR_OFF"
_DEBUG_="$COLOUR_YELLOW[DEBUG]$COLOUR_OFF"

stdin="/proc/$$/fd/0"
stdout="/proc/$$/fd/1"
stderr="/proc/$$/fd/2"
stdnull="/dev/null"

print() {
    if ! $opt_quiet
    then
        echo -e $@ >&$stdout
    fi
}
print_err() {
    if ! $opt_quiet
    then
        echo -e $@ >&$stderr
    fi
}

tm4c_version="2.1.4.178"
tm4c_dir="TivaWare_C_Series-${tm4c_version}"
tm4c_driver="${tm4c_dir}/driverlib/gcc/libdriver.a"

## Check for TM4C Driver Blob
if [ ! -e "${tm4c_driver}" ]; then
  ## Check for TM4C Driver source directory
  if [ -d "${tm4c_dir}" ]; then
    ## source directory exists so compile Driver Blob
    print "$_INFO_ Compiling TivaWare Driver.."
    cd ${tm4c_dir}/driverlib
    make &>/dev/null \
    && [ -e "${tm4c_driver}" ] \
    && {
      print "$_INFO_ TivaWare Driver Compiled.";
    } || {
      print_err "$_ERROR_ There were errors in building the TivaWare Driver :(";
      exit 1;
    }
  ## Check for downloadable TM4C Driver Blob
  elif [ ! -z "$TM4C_LIBDRIVER_URL" ]; then
    ## URL is set on environment variable so download
    print "$_INFO_ Downloading TivaWare Driver.."
    wget -q "$TM4C_LIBDRIVER_URL-${tm4c_version}.tar.bz2" -O tm4c.tar.bz2 \
    && tar -xjf tm4c.tar.bz2 && rm tm4c.tar.bz2 \
    && print "$_INFO_ TivaWare Driver Downloaded."
  else
    print_err "$_ERROR_ TivaWare Driver Library not found! Please download these from TI following README.md"
    exit 1
  fi
fi

print "$_INFO_ Using TivaWare Driver Library ${tm4c_version}";
exit 0;