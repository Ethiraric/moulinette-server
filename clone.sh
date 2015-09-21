#!/bin/bash

# Check arguments
if [[ $# -ne 5 ]]
then
    echo "Usage : ./clone.sh <USER> <REPO> <CLONE DIR> <TESTS DIR> <CLONE LOGIN>"
    exit 1
fi

login=$1
repo=$2
clonedir=$3
testsdir=$4
clonelogin=$5

# Check login
regex='^[a-z0-9_-]{1,6}_[a-z0-9_-]$'
if [[ ! $login =~ $regex ]]
then
    echo "Illegal username"
    exit 1
fi

# Check if there is a testsuite for the repo
if [[ ! -d $testsdir/$repo ]]
then
    echo "No tests for $repo"
    exit 1
fi

# Check for the clone subdirectory
if [[ ! -d $clonedir ]]
then
    mkdir -p $clonedir
fi

cd $clonedir

# Create subdirectory
if [[ ! -d $login ]]
then
    mkdir -p $login
fi
cd $login

# Try to clone / pull
if [[ ! -d $repo ]]
then
    git clone $clonelogin@git.epitech.eu:/$login/$repo
    if [[ $? -ne 0 ]]
    then
	echo "Failed to clone $login/$repo"
	exit 1
    fi
    cd $repo
else
    cd $repo
    git pull
    if [[ $? -ne 0 ]]
    then
	echo "Failed to pull $login/$repo"
	exit 1
    fi
fi

# Copy tests
if [[ -d .tests ]]
then
    rm -rf .tests
fi

mkdir -p .tests
cp ../../../$testsdir/$repo/* ./.tests -r


echo "Clone OK"

# Exit successfully
exit 0
