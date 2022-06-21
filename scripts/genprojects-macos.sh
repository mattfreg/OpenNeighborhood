#!/bin/bash

SCRIPT_ABSOLUTE_PATH=`realpath "$BASH_SOURCE"`
SCRIPT_DIR=`dirname "$SCRIPT_ABSOLUTE_PATH"`
ROOT_DIR=`dirname "$SCRIPT_DIR"`

$ROOT_DIR/tools/premake/bin/premake5_macos --file=$ROOT_DIR/premake5.lua gmake2
