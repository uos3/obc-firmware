#!/usr/bin/env /bin/bash

source_dir="$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)"
cd "$source_dir"

echo 'Checking git branch'
# Exit unless we're on the master branch
BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [[ "$BRANCH" != "master" ]]; then
  echo 'Git branch is not master. Branch is ${BRANCH}';
  exit 0;
fi

# Exit with nonzero exit code if anything fails
set -e

echo 'Decrypting SSH key'
# Get the deploy key by using Travis's stored variables to decrypt travis-deploy.enc
openssl aes-256-cbc -iv ${DEPLOY_KEY_IV} -K ${DEPLOY_KEY_KEY} -in travis-deploy.enc -out deploy_key -d
chmod 600 deploy_key
eval `ssh-agent -s`
ssh-add deploy_key

echo 'Cloning repository'
# Get the current gh-pages branch
git clone -b gh-pages $GH_REPO_REF gh-pages
cd gh-pages

echo 'Configuring git'
# Set the push default to simple i.e. push only the current branch.
git config --global push.default simple
# Pretend to be an user called Travis CI.
git config user.name "Travis CI"
git config user.email "travis@travis-ci.org"

# Remove everything currently in the gh-pages branch.
# GitHub is smart enough to know which files have changed and which files have
# stayed the same and will only update the changed files. So the gh-pages branch
# can be safely cleaned, and it is sure that everything pushed later is the new
# documentation.
rm -rf *

# Need to create a .nojekyll file to allow filenames starting with an underscore
# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
# Presumably this is only needed when the SHORT_NAMES option in Doxygen is set
# to NO, which it is by default. So creating the file just in case.
echo "" > .nojekyll

################################################################################
##### Generate the Doxygen code documentation and log the output.          #####
echo 'Generating Doxygen code documentation...'

# Redirect both stderr and stdout to the log file AND the console.
doxygen ../doxyfile 2>&1 | tee doxygen.log

# Bail out if there's no changes
if [ -z `git diff --exit-code` ]; then
    echo "No doc changes on this push; exiting."
    exit 0
fi

################################################################################
##### Upload the documentation to the gh-pages branch of the repository.   #####
# Only upload if Doxygen successfully created the documentation.
# Check this by verifying that the html directory and the file html/index.html
# both exist. This is a good indication that Doxygen did it's work.
if [ -f "index.html" ]; then

    echo 'Uploading documentation to the gh-pages branch...'
    # Add everything in this directory (the Doxygen code documentation) to the
    # gh-pages branch.
    # GitHub is smart enough to know which files have changed and which files have
    # stayed the same and will only update the changed files.
    git add --all

    # Commit the added files with a title and description containing the Travis CI
    # build number and the GitHub commit reference that issued this build.
    git commit -m "Deploy to GitHub Pages: ${TRAVIS_COMMIT}"

    # Push to the remote gh-pages branch.
    # The ouput is redirected to /dev/null to hide any sensitive credential data
    # that might otherwise be exposed.
    git push # > /dev/null 2>&1
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    exit 1
fi