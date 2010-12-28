#!/bin/bash
rm -rf /tmp/blobseer

killall provider
killall sdht
killall ruby
killall pmanager
killall vmanager
killall java
pkill provider
pkill sdht
pkill ruby
pkill pmanager
pkill vmanager
pkill java
kill -9 `ps -u $USER | grep "blobseer" | awk '{ print $1 }'`
ps ax | grep blobseer  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }'
ps ax | grep java  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }'

