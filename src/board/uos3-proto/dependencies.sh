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

if [ ! -d "${tm4c_dir}" ]; then
  print_err "$_ERROR_ TM4C Drivers not found! Please download these from TI following README.md"
  exit 2
fi
if [ ! -e "${tm4c_driver}" ]; then
  print_err "$_ERROR_ TM4C Drivers not compiled!"
  print "$_INFO_ TM4C Drivers Compiling now - please wait.."
  cd ${tm4c_dir}/driverlib
  make &>/dev/null \
  && print "$_INFO_ TM4C Drivers Compiled." \
  || print_err "$_ERROR_ There were errors in building the TM4C Drivers :("
  cd ${source_dir}
fi