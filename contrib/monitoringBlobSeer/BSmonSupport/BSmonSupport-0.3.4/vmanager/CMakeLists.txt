
#find the path to the JAVA commands
find_package(JAVA)
find_package(ApmonJAVA)

set(VMONITOR_CLASSPATH ${APMON_JAVA_LIBRARIES})

ADD_CUSTOM_COMMAND(
	    OUTPUT *.class
	    COMMAND ${JAVA_COMPILE}  -classpath ${VMONITOR_CLASSPATH}
				*.java
		)
	  

ADD_CUSTOM_TARGET(vmanagerParser ALL
	    DEPENDS *.class)


#############################
#Define a make clean command
ADD_CUSTOM_COMMAND(
	    OUTPUT cl
	    PRE_BUILD
	    COMMAND rm *.class
		)

ADD_CUSTOM_TARGET(clean
	    DEPENDS cl)

	  



