
set(VMONITOR_CLASSPATH ${APMON_LIBRARIES})

ADD_CUSTOM_COMMAND(
	    OUTPUT *.class
	    COMMAND ${JAVA_COMPILE}  -classpath ${VMONITOR_CLASSPATH}
				*.java
		)
	  

ADD_CUSTOM_TARGET(vmanagerParser ALL
	    DEPENDS *.class)





