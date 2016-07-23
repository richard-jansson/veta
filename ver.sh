#!/bin/bash
MAJOR=0.1
BRANCH=$( git branch | grep "*" | cut -d " " -f2)
MINOR=$( git rev-list --count HEAD)

VERSION=$MAJOR.$MINOR-$BRANCH
COMMIT=$( git log | head -n1 | cut -d " " -f2)

echo  "#define VERSION \"$VERSION\""
echo  "#define COMMIT \"$COMMIT\""
echo  "#define BUILD \"$VERSION $COMMIT\""
