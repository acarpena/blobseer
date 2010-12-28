#!/bin/bash

killall NamespaceManager
pkill NamespaceManager

kill -9 `ps -u $USER | grep "NamespaceManager" | awk '{ print $1 }'`
ps ax | grep NamespaceManager  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }'
