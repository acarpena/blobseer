#!/bin/bash

if [ "${HADOOP_HOME}" == "" ] 
then
    echo "\$HADOOP_HOME is undefined. It needs to point to hadoop home dir."
    exit 1
fi    

OUTPUT=$HADOOP_HOME/build/tests

javac=$JAVA_HOME/bin/javac

files=$(dir -d *.java)
for i in $files; do
      $javac $i -cp $HADOOP_HOME/build/classes:$HADOOP_HOME/build/tests:.
done
classes=$(dir -d *.class)
for i in $classes; do
      mv $i $OUTPUT
done
