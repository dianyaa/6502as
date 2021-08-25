#!/bin/sh

PROJECT_ROOT=$(dirname "$0")

build()
{
        cd $PROJECT_ROOT/build || exit
        make
}

setup()
{
	echo "Generating build directory"
        cmake -B $PROJECT_ROOT/build -S $PROJECT_ROOT
        build
}

clean()
{
	echo "Removing build directory"
	rm -rf $PROJECT_ROOT/build
	exit
}

if [ "$1" = "clean" ]; then
	clean
fi

if [ -d "$PROJECT_ROOT/build" ]; then
        build
else
        setup
fi
