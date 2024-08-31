#!/bin/bash

false | true
echo $?
set -o pipefail
false | true
echo $?

exit 0
