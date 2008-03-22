#!/bin/sh
echo "> Removing pesky \"*~\" backups..."
find . -name '*~' -exec rm {} \;