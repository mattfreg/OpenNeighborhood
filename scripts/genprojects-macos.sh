#!/bin/bash

SCRIPT_DIR=`dirname "$BASH_SOURCE"`

$SCRIPT_DIR/../vendor/premake/bin/premake5_macos --file=$SCRIPT_DIR/../PremakeConfig/main.lua gmake2
