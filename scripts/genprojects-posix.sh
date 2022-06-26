#!/bin/bash

SCRIPT_ABSOLUTE_PATH=`realpath "$BASH_SOURCE"`
SCRIPT_DIR=`dirname "$SCRIPT_ABSOLUTE_PATH"`
ROOT_DIR=`dirname "$SCRIPT_DIR"`
PREMAKE_EXECUTABLE_PATH=$ROOT_DIR/tools/premake/premake5

if [[ ! -f "$PREMAKE_EXECUTABLE_PATH" ]]; then
    echo "Downloading premake..."
    "$SCRIPT_DIR/download-premake-posix.sh"
fi

"$PREMAKE_EXECUTABLE_PATH" --file="$ROOT_DIR/premake5.lua" gmake2
