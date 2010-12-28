# The name of this project is "MonitoringBlobSeer". CMakeLists files in this project can
# refer to the root source directory of the project as ${MON_SOURCE_DIR} 
cmake_minimum_required (VERSION 2.6)
project (BS_MON)


#set the path to the MonALISA repository
set(REPOS_ROOT "~/Work/BlobMonitoring/MLrepository")

#uncomment the next line and set the path to JAVA_HOME, 
#if it is not set as an environment variable
#set(JAVA_HOME "/usr/java")

#uncomment the next line and set the path to other libraries needed for the repository
set(LIBRARY_PATH "~/lib")



# set the path to the cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#find the path to the JAVA commands
find_package(JAVA)
find_package(Repos)


###########################################
##-----Create needed directories---------##
###########################################
ADD_CUSTOM_COMMAND(
	    OUTPUT dirs
	    COMMAND rm -drf build
	    COMMAND mkdir build
	    COMMAND rm -drf lib
	    COMMAND mkdir lib
		)
ADD_CUSTOM_TARGET(CreateDirs 
	    DEPENDS dirs)



###########################################
##-----Compile the Chart classes---------##
###########################################
set(REPOS_CLASSPATH "${REPOS_LIBRARIES}:${JFREECHART_LIBRARIES}:${JCOMMON_LIBRARIES}:${POSTGRES_LIBRARIES}:${SERVLET_LIBRARIES}")

# source directories
set (SRC_CHART_BEAN      "src/mon/charts/bean/*.java")
set (SRC_CHART_SERVLET   "src/mon/charts/servlet/*.java")
set (SRC_UTIL_DB         "src/mon/util/db/*.java")
set (SRC_UTIL_BLOB       "src/mon/util/blob/*.java")

ADD_CUSTOM_COMMAND(
	    OUTPUT buildcharts
	    COMMAND ${JAVA_COMPILE}  
			-classpath ${REPOS_CLASSPATH}
			-d build		
			${SRC_CHART_SERVLET} 
			${SRC_UTIL_DB} 
			${SRC_UTIL_BLOB}  
			${SRC_CHART_BEAN}
		)
	 
ADD_CUSTOM_TARGET(charts 
	    DEPENDS dirs buildcharts)




###########################################
##-----Compile the Filter classes--------##
###########################################
set(FILTERS_CLASSPATH "${REPOS_LIBRARIES}:${POSTGRES_LIBRARIES}:${FILTER_LIBRARIES}")

# source directories
set (SRC_FILTER	    	 "src/mon/filters/*.java")
set (SRC_UTIL		 "src/mon/filters/util/*.java")
set (SRC_UTIL_DB         "src/mon/util/db/*.java")
set (SRC_PROV		 "src/mon/filters/providermonitor/*.java")
set (SRC_VMAN		 "src/mon/filters/vmanagermonitor/*.java")
set (SRC_BLOB		 "src/mon/filters/blobscreatemonitor/*.java")

ADD_CUSTOM_COMMAND(
	    OUTPUT buildfilters
	    COMMAND ${JAVA_COMPILE}  
			-classpath ${FILTERS_CLASSPATH}
			-d build		
			${SRC_UTIL} 
			${SRC_UTIL_DB} 
			${SRC_PROV}  
			${SRC_VMAN}
			${SRC_BLOB}  
			${SRC_FILTER}
		)
	 
ADD_CUSTOM_TARGET(filters 
	    DEPENDS dirs buildfilters)




###########################################
##-----Create the .jar Chart archives----##
###########################################
set (CLASS_CHART_BEAN      "mon/charts/bean/")
set (CLASS_CHART_SERVLET   "mon/charts/servlet/")
set (CLASS_UTIL_DB         "mon/util/db/")
set (CLASS_UTIL_BLOB       "mon/util/blob/")

ADD_CUSTOM_COMMAND(
	    OUTPUT createChartJar
	    COMMAND ${JAVA_ARCHIVE}
			-cf lib/BSMonitoring.jar  
			-C  build ${CLASS_CHART_SERVLET} 
			-C  build ${CLASS_UTIL_DB} 
			-C  build ${CLASS_UTIL_BLOB}  
			-C  build ${CLASS_CHART_BEAN}
	    DEPENDS buildcharts		
		)
	 
ADD_CUSTOM_TARGET(CreateChartLibraries 
	    DEPENDS buildcharts  createChartJar)



#############################################
##-----Create the .jar FILTERS archives----##
#############################################
set (CLASS_FILTER	 "mon/filters/")
set (CLASS_UTIL_DB       "mon/util/db/")

ADD_CUSTOM_COMMAND(
	    OUTPUT createFiltersJar
	    COMMAND ${JAVA_ARCHIVE}
			-cf lib/BSfilter.jar  
			-C build ${CLASS_FILTER}
			-C build ${CLASS_UTIL_DB}
	    DEPENDS buildfilters 
		)
	 
ADD_CUSTOM_TARGET(CreateFilterLibraries 
	    DEPENDS buildfilters  createFiltersJar)


############################################
##----------The MAKE ALL target-----------##
############################################

ADD_CUSTOM_TARGET(all ALL 
	    DEPENDS dirs createChartJar createFiltersJar)



############################################
##-----Install files in the repository----##
############################################
set (LIB_CHARTS      "lib/BSMonitoring.jar")
set (REPOSITORY_CHARTS_PATH   "${REPOS_ROOT}/tomcat/common/lib")

set (FILTERS     "lib/BSfilter.jar")
set (REPOSITORY_FILTERS_PATH   "${REPOS_ROOT}/tomcat/common/lib")

set (JSP_FILES      "html/*.jsp")
set (REPOSITORY_JSP_PATH  "${REPOS_ROOT}/tomcat/webapps/ROOT")

set (CONF_FILES "conf")


ADD_CUSTOM_COMMAND(
	    OUTPUT inst
	    COMMAND cp	-f		# install the Charts archive
		${LIB_CHARTS}
		${REPOSITORY_CHARTS_PATH} 
	    COMMAND cp -f		# install the Filters archive
		${FILTERS}
		${REPOSITORY_FILTERS_PATH}
            COMMAND /bin/cp		# install JSP files
		${JSP_FILES}
		${REPOSITORY_JSP_PATH} 
	    COMMAND /bin/cp		# install configuration files
		${CONF_FILES}/web.xml
		${REPOS_ROOT}/tomcat/webapps/ROOT/WEB-INF/web.xml
	    COMMAND /bin/cp			
		${CONF_FILES}/menu.js
		${REPOS_ROOT}/tomcat/webapps/ROOT/js/menu.js
	    COMMAND /bin/cp
		${CONF_FILES}/website_conf/*.properties
		${REPOS_ROOT}/conf/website_config_files/
	    COMMAND cp
		${CONF_FILES}/App.properties
		${REPOS_ROOT}/JStoreClient/conf/App.properties
		)


	 
ADD_CUSTOM_TARGET(install
	    DEPENDS inst)




############################################
##----------Clean the installation--------##
############################################

ADD_CUSTOM_COMMAND(
	    OUTPUT cl
	    COMMAND rm -drf
	    	build
	    COMMAND rm -drf
	    	lib
		)
ADD_CUSTOM_TARGET(clean
	    DEPENDS cl)


	  
