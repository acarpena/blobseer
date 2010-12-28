#!/bin/bash

FILE=$1

cat $1 | awk  '
	{
	if (match($1, "rennes") > 0)
		{
		print $1
		next
		}
	if (match($1, "sophia") > 0)
        { 
        print $1
        next
        }
    if (match($1, "lille") > 0)
        {
        print $1
        next
        }
	if (match($1, "nancy") > 0)
        {
        print $1
        next
        }
	if (match($1, "orsay") > 0)
        {
        print $1
        next
        }
	if (match($1, "bordeaux") > 0)
        {
        print $1
        next
        }


	s=$0 "."
	if (match($1,"azur") > 0)
		{ s=s "sophia"}
	if (match($1,"helios") > 0)
                { s=s "sophia"}
	if (match($1,"sol") > 0)
                { s=s "sophia"}

	if (match($1,"paramount") > 0)
                { s=s "rennes"}
	if (match($1,"paradent") > 0)
                { s=s "rennes"}
	if (match($1,"paraquad") > 0)
                { s=s "rennes"}
	if (match($1,"parapide") > 0)
                { s=s "rennes"}


	if (match($1,"gdx") > 0)
                { s=s "orsay"}
	else
	{
	if (match($1,"netgdx") > 0)
                { s=s "orsay"}
	}

	if (match($1,"chuque") > 0)
                { s=s "lille"}
	if (match($1,"chicon") > 0)
                { s=s "lille"}
	if (match($1,"chinqchint") > 0)
                { s=s "lille"}
	if (match($1,"chti") > 0)
                { s=s "lille"}

    if (match($1,"bordemer") > 0)
                { s=s "bordeaux"}
    if (match($1,"bordeplage") > 0)
                { s=s "bordeaux"}
    if (match($1,"bordereau") > 0)
                { s=s "bordeaux"}
    if (match($1,"borderline") > 0)
                { s=s "bordeaux"}

    if (match($1,"grelon") > 0)
                { s=s "nancy"}
    if (match($1,"griffon") > 0)
                { s=s "nancy"}
    if (match($1,"grillon") > 0)
                { s=s "nancy"}


	sinit=$0 "."
	if (s!=sinit)
	  {
	  s=s ".grid5000.fr"
	  print s
	  }
    else
        {
        print $0	
        }
    }'

