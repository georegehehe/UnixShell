#!/bin/bash

# Clean everything except grade.py,clean.sh,runOneTime.sh in test-scripts
# myshell.c and makefile just in case, not delete them
# The reason we do this is last year, we got some other weird output affect result, so create this rubust version

curdir=`pwd -P`
if $( echo $curdir | grep --quiet 'p4shell/test-scripts' )
then
   echo	"You are in the right directory"
   allFiles=`ls`
   for file in $allFiles
   do
     if [ "$file" == "myshell.c" ] ||
        [ "$file" == "grade.py" ] ||
        [ "$file" == "clean.sh" ] ||
	[ "$file" == "runOneTest.sh" ] ||
        [ "$file" == "myshell" ] ||
        [ "$file" == "Makefile" ]; then

        echo "Don't delete $file"
        continue
    fi
     if [ "$file" == "out" ] ; then
	echo "deleting directory $file ..."
	rm -f -r $file
     else
	echo "deleting $file ..."
     	rm -f $file
    fi
done
else
    echo "ERROR: You are not in the right directory because you should be in your p4shell/test-scripts directory. Exiting"
    exit
fi
