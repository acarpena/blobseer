#!/bin/bash
rm -rf /tmp/blobseer
rm -rf /tmp/*hadoop*

killall provider
killall sdht
#killall ruby
killall pmanager
killall vmanager
killall java
killall -9 `ps -u $USER | grep "\.sh" | awk '{ print $1 }'`

pkill provider
pkill sdht
#killall ruby
pkill pmanager
pkill vmanager
pkill java

