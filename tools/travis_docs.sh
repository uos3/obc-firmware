#!/usr/bin/env bash

COLOUR_GREEN='\033[0;32m'
COLOUR_YELLOW='\033[1;33m'
COLOUR_PURPLE='\033[0;35m'
COLOUR_RED='\033[0;31m'
COLOUR_OFF='\033[0;00m'
CLEAR_LINE='\033[2K'

_ERROR_="$COLOUR_RED[ERROR]$COLOUR_OFF"
_INFO_="$COLOUR_PURPLE[INFO]$COLOUR_OFF"
_DEBUG_="$COLOUR_YELLOW[DEBUG]$COLOUR_OFF"

print() {
    echo -e $@
}

# Change to script source directory
source_dir="$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)"
cd "$source_dir";

print "$_DEBUG_ Decrypting SSH key"
# Get the deploy key by using Travis's stored variables to decrypt travis-deploy.enc
openssl aes-256-cbc -iv ${DEPLOY_KEY_IV} -K ${DEPLOY_KEY_KEY} -in travis-deploy.enc -out deploy_key -d
chmod 600 deploy_key
eval `ssh-agent -s`
ssh-add deploy_key

print "$_DEBUG_ Cloning repository"
# Get the current gh-pages branch
git clone -b gh-pages ssh://git@github.com:/${TRAVIS_REPO_SLUG} gh-pages
cd gh-pages

print "$_DEBUG_ Configuring git"
# Set the push default to simple i.e. push only the current branch.
git config --global push.default simple
# Pretend to be an user called Travis CI.
git config user.name "Travis CI"
git config user.email "travis@travis-ci.org"

# Remove everything currently in the gh-pages branch.
rm -rf *

# Need to create a .nojekyll file to allow filenames starting with an underscore
echo "" > .nojekyll

print "$_DEBUG_ Generating Doxygen code documentation"
# Redirect both stderr and stdout to the log file and the console.
doxygen ../doxyfile 2>&1 | tee doxygen.log

# Generate html coverage report
$(cd ${source_dir}/../ && ./test)
coverageFile="$(ls -t ${source_dir}/../builds/*-coverage.info | head -1)"
genhtml ${coverageFile} --output-directory coverage/

# Bail out if there's no changes
git diff -s --exit-code
if [ $? -eq 0 ]; then
    print "$_INFO_ No doc changes on this push; exiting."
    exit 0
fi

# Only upload if Doxygen successfully created the documentation.
if [ -f "index.html" ]; then
    print "$_DEBUG_ Uploading documentation to the gh-pages branch."
    git add --all
    git commit -m "Deploy to GitHub Pages: ${TRAVIS_COMMIT}"
    git push
	print "$_INFO_ Doxygen successful."
else
    print "$_ERROR_ No documentation (html) files have been found!"
    exit 1
fi