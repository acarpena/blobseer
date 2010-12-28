/*
 * globus_gridftp_server_blobseer.h
 *
 *  Created on: Mar 2, 2010
 *      Author: vdinh@irisa.fr
 */

#ifndef GLOBUS_GRIDFTP_SERVER_BLOBSEER_H_
#define GLOBUS_GRIDFTP_SERVER_BLOBSEER_H_



GlobusDebugDeclare(GLOBUS_GRIDFTP_SERVER_BLOBSEER); //declare a debug. It must be have a define statement at the begining of the c file .

#define GlobusGFSBlobseerDebugPrintf(level, message)                                \
    GlobusDebugPrintf(GLOBUS_GRIDFTP_SERVER_BLOBSEER, level, message)


#define GlobusGFSBlobseerDebugInfo(_msg)                                            \
    GlobusGFSBlobseerDebugPrintf(                                                   \
        GLOBUS_GFS_DEBUG_INFO,                                              \
        ("[%s] %s\n", _gfs_name, _msg))

#define GlobusGFSBlobseerDebugEnter()                                               \
    GlobusGFSBlobseerDebugPrintf(                                                   \
        GLOBUS_GFS_DEBUG_TRACE,                                             \
        ("[%s] Entering\n", _gfs_name))

#define GlobusGFSBlobseerDebugExit()                                                \
    GlobusGFSBlobseerDebugPrintf(                                                   \
        GLOBUS_GFS_DEBUG_TRACE,                                             \
        ("[%s] Exiting\n", _gfs_name))

#define GlobusGFSBlobseerDebugState(_state)                                         \
    GlobusGFSBlobseerDebugPrintf(                                                   \
        GLOBUS_GFS_DEBUG_INFO,                                             \
        ("[%s] State: %d\n", _gfs_name, _state))

#define GlobusGFSBlobseerDebugExitWithError()                                       \
    GlobusGFSBlobseerDebugPrintf(                                                   \
        GLOBUS_GFS_DEBUG_TRACE,                                             \
        ("[%s] Exiting with error\n", _gfs_name))

#endif /* GLOBUS_GRIDFTP_SERVER_BLOBSEER_H_ */


static
void
GlobusGFSBlobseerLogInfo(_msg)
{
	 globus_gfs_log_message(
	        GLOBUS_GFS_LOG_INFO,
	       _msg + "\n");
}
