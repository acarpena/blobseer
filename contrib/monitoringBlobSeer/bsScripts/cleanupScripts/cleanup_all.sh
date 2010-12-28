#!/bin/bash

echo
echo "Cleaning the database server..."
killall bsmon
pkill bsmon
kill -9 `ps ax | grep bsmon | cut -d ' ' -f 1 ` > /dev/null
kill -9 `ps ax | grep bsmon | cut -d ' ' -f 2 ` > /dev/null
ps ax | grep bsmon  | cut -d ' ' -f 2 | awk '{com="kill -9 " $0; system(com) }' > /dev/null
ps ax | grep bsmon  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }' > /dev/null


#killall postgres
#pkill postgres
#kill -9 `ps ax | grep postgres | cut -d ' ' -f 1 ` > /dev/null
#kill -9 `ps ax | grep postgres | cut -d ' ' -f 2 ` > /dev/null
#ps ax | grep postgres  | cut -d ' ' -f 2 | awk '{com="kill -9 " $0; system(com) }' > /dev/null
#ps ax | grep postgres  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }' > /dev/null

echo
echo "Cleaning the repos&detection..."
killall java
pkill java
kill -9 `ps ax | grep java | cut -d ' ' -f 1 ` > /dev/null
kill -9 `ps ax | grep java | cut -d ' ' -f 2 ` > /dev/null
ps ax | grep java  | cut -d ' ' -f 2 | awk '{com="kill -9 " $0; system(com) }' > /dev/null
ps ax | grep java  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }' > /dev/null

#rm -rf /local/pgsql
rm -rf /local/log.out

echo
echo "Removing crons..."
crontab -r

echo "IRISA nodes cleaned"




