#!/bin/bash

kill -9 `ps ax | grep bsmon | cut -d ' ' -f 1 `
kill -9 `ps ax | grep bsmon | cut -d ' ' -f 2 `
ps ax | grep bsmon  | cut -d ' ' -f 2 | awk '{com="kill -9 " $0; system(com) }'
ps ax | grep bsmon  | cut -d ' ' -f 1 | awk '{com="kill -9 " $0; system(com) }'

echo "DB server stopped"




