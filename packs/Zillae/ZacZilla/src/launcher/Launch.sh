#!/bin/sh
#set -vx
exec >> /opt/Zillae/ZacZilla/Misc/Launch.log 2>&1

cd /opt/Zillae/ZacZilla
exec /opt/Zillae/ZacZilla/ZacZilla
