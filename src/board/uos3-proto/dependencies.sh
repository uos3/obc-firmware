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

tm4c_dir="TivaWare_C_Series-2.1.4.178"
tm4c_driver="TivaWare_C_Series-2.1.4.178/driverlib/gcc/libdriver.a"

## Check for TM4C Driver Blob
if [ ! -e "${tm4c_driver}" ]; then
  ## Check for downloadable TM4C Driver Blob
  if [ ! -z "$TM4C_LIBDRIVER_URL" ]; then
    print "$_INFO_ Downloading TM4C Drivers.."
    wget -q "$TM4C_LIBDRIVER_URL" -O tm4c.tar.bz2 \
    && tar -xjf tm4c.tar.bz2 && rm tm4c.tar.bz2 \
    && print "$_INFO_ TM4C Drivers Downloaded." \
    && exit 0
  fi
  ## Check for TM4C Driver source directory
  if [ ! -d "${tm4c_dir}" ]; then
    print_err "$_ERROR_ TM4C Drivers not found! Please download these from TI following README.md"
    exit 1
  else
    ## source directory exists so compile Driver Blob
    print "$_INFO_ TM4C Drivers Compiling now - please wait.."
    cd ${tm4c_dir}/driverlib
    make &>/dev/null \
    && [ -e "${tm4c_driver}" ] \
    && {
      print "$_INFO_ TM4C Drivers Compiled.";
    } || {
      print_err "$_ERROR_ There were errors in building the TM4C Drivers :(";
      exit 1;
    }
  fi
else
  print "$_INFO_ TM4C Drivers Found.";
  exit 0;
fi

exit 1;