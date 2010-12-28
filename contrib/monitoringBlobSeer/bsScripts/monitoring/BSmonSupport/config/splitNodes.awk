BEGIN 	{ 
	nr=0 
	}
{ 
nr = nr + 1;
FILE = output "-" nr ".txt"
print $0 >> FILE

if ( nr >= lines)
	nr = 0
}

