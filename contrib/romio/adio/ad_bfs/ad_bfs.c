/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *	Written by Viet-Trung TRAN
 */

#include "ad_bfs.h"

/* adioi.h has the ADIOI_Fns_struct define */
#include "adioi.h"

struct ADIOI_Fns_struct ADIO_BFS_operations = {
    ADIOI_BFS_Open, /* Open */
    ADIOI_GEN_OpenColl, /* OpenColl */
    ADIOI_BFS_ReadContig, /* ReadContig */
    ADIOI_BFS_WriteContig, /* WriteContig */
    ADIOI_BFS_ReadStridedColl, /* ReadStridedColl */
    ADIOI_BFS_WriteStridedColl, /* WriteStridedColl */
    ADIOI_BFS_SeekIndividual, /* SeekIndividual */
    ADIOI_BFS_Fcntl, /* Fcntl */
    ADIOI_BFS_SetInfo, /* SetInfo */
    ADIOI_BFS_ReadStrided, /* ReadStrided */
    ADIOI_BFS_WriteStrided, /* WriteStrided */
    ADIOI_BFS_Close, /* Close */
    ADIOI_BFS_IreadContig, /* IreadContig */
    ADIOI_BFS_IwriteContig, /* IwriteContig */
    ADIOI_BFS_ReadDone, /* ReadDone */
    ADIOI_BFS_WriteDone, /* WriteDone */
    ADIOI_BFS_ReadComplete, /* ReadComplete */
    ADIOI_BFS_WriteComplete, /* WriteComplete */
    ADIOI_BFS_IreadStrided, /* IreadStrided */
    ADIOI_BFS_IwriteStrided, /* IwriteStrided */
    ADIOI_BFS_Flush, /* Flush */
    ADIOI_BFS_Resize, /* Resize */
    ADIOI_BFS_Delete, /* Delete */
    ADIOI_GEN_Feature, /* Features */
};
