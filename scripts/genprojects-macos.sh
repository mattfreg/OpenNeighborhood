#!/bin/bash

SCRIPT_DIR=`dirname "$BASH_SOURCE"`

$SCRIPT_DIR/../tools/premake/bin/premake5_macos gmake2
