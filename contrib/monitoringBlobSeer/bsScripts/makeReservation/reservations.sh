#!/bin/bash


#reserve nodes for the MonALISA services
#oargridsub -t deploy rennes:rdef="nodes=10" -w 1:55:00 
#oargridsub -t deploy rennes:rdef="/nodes=10" -w 2:00:00 -s "2009-01-27 10:30:00"


#reserve nodes for BlobSeer
#oargridsub orsay:rdef="/nodes=300"  -w 1:55:00 -v 

oargridsub -w 2:00:00 -s "2010-01-27 10:30:00" gdx:rdef="/nodes=270" -v
#oargridsub -w 61:00:00 -s "2010-01-15 19:00:00" gdx:rdef="/nodes=270" -v
#oargridsub -w 8:00:00 -s "2009-12-08 22:50:00"  bordeaux:rdef="/nodes=80", azur:rdef="/nodes=10",  sol:rdef="/nodes=20", rennes:rdef="/nodes=120" -v


