#!/bin/bash

# This script tags the state of the current branch and optionally pushes
# tags to the remote.  It also configures the local git repo to include tags
# during git push.

date=`date +%Y.%m.%d_%H.%M.%S`
pid=$$

# Make sure you're in a git repository with a file and initial commit
out=`git ls-tree --full-tree --name-only -r HEAD 2> /dev/null`
if [[ $? -ne 0 || "$out" = "" ]]; then
    echo "Error: no committed files in repository.  Please add files to your repository and commit."
    exit 1
fi

base=`git rev-parse --show-toplevel`

# Ignore signals, so users don't stop in an intermediate state
trap "" SIGINT SIGQUIT SIGTSTP

# Save modification times of files with uncommitted changes.  The 2nd sed command
# deletes surrounding double quotes; the 3rd sed command removes a level of
# backslashes in backslashed characters.
files=`git status -uno --porcelain | sed -e "s/^...//" -e 's/^\"\\(.*\\)\"$/\1/' -e 's/\\\\\(.\\)/\1/g'`
IFS=$'\n'	# split on newline only
for file in $files; do
    if [[ -e $base/$file ]]; then
	(cd $base; touch -r $file $file.autotag.$pid)
    fi
done

# Stash local changes
stash1=`git stash list`
git stash > /dev/null
stash2=`git stash list`
if [[ "$stash1" != "$stash2" ]]; then
    git stash apply > /dev/null
fi

# Create temporary commit
git commit -am compile-${date} --allow-empty > /dev/null

if [[ $? -eq 0 ]]; then
    # create tag for the temporary commit (fail silently if tag already exists)
    git tag -a compile-${date} -m "" >& /dev/null

    # Push tag (if requested on the command line).  Allow this to be killed
    # (without killing autotag.sh).
    if [[ "$1" = "push" ]]; then
	(trap - SIGINT SIGQUIT SIGTSTP; git push --tags)
    fi

    # undo temporary commit
    git reset --hard HEAD~ > /dev/null
fi

# Restore local changes (if any)
if [[ "$stash1" != "$stash2" ]]; then
    git stash pop > /dev/null
fi

# Restore modification times
for file in $files; do
    if [[ -e $base/$file.autotag.$pid ]]; then
	(cd $base; touch -r $file.autotag.$pid $file; rm $file.autotag.$pid)
    fi
done

# Configure git to push tags (in addition to the current branch)
config=`git config --get-all remote.origin.push`
if [[ ! $config =~ refs/tags/\* ]]; then
    git config --add remote.origin.push refs/tags/*
fi
if [[ ! $config =~ HEAD ]]; then
    git config --add remote.origin.push HEAD
fi
