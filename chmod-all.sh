#!/bin/sh
echo "> Setting all script files to +rwx..."
find . -name '*.sh' -exec chmod ugo+rwx {} \;