/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *
 *   Copyright (C) 1997 University of Chicago. 
 *   See COPYRIGHT notice in top-level directory.
 */


/* header file for MPI-IO implementation. not intended to be
   user-visible */ 

#ifndef MPIOIMPL_INCLUDE
#define MPIOIMPL_INCLUDE

#include "adio.h"
#include "mpio.h"

/* FIXME: We should be more restricted in what we include from MPICH2
   into ROMIO */
#ifdef ROMIO_INSIDE_MPICH2
#include "mpiimpl.h"
#include "mpiimplthread.h"

/* Use the routine versions of the nest macros, to avoid requiring 
   access to the MPIR_Process and MPIR_Thread structures */
#ifdef MPIR_Nest_incr
#undef MPIR_Nest_incr
#undef MPIR_Nest_decr
#endif

#ifndef MPICH_DEBUG_NESTING
/* These implement the nesting support expected by MPICH2; the primary
   purpose of these is to cause MPI routines to return error codes 
   instead of invoking an error handler (which would be incorrect,
   since the relevant MPI_File error handler should be invoked). */
void MPIR_Nest_incr_export(void);
void MPIR_Nest_decr_export(void);
#define MPIR_Nest_incr() MPIR_Nest_incr_export()
#define MPIR_Nest_decr() MPIR_Nest_decr_export()
#else
/* These implement the nesting support with additional code to 
   provide traces of nest increments and decrements, which are very
   helpful in finding errors in properly updating the nesting values */
void MPIR_Nest_incr_export_dbg(const char *, int);
void MPIR_Nest_decr_export_dbg(const char *, int);
#define MPIR_Nest_incr() MPIR_Nest_incr_export_dbg(__FILE__,__LINE__)
#define MPIR_Nest_decr() MPIR_Nest_decr_export_dbg(__FILE__,__LINE__)
#endif

#else /* not ROMIO_INSIDE_MPICH2 */
/* Any MPI implementation that wishes to follow the thread-safety and
   error reporting features provided by MPICH2 must implement these 
   four functions.  Defining these as empty should not change the behavior 
   of correct programs */
#define MPIU_THREAD_CS_ENTER(x,y)
#define MPIU_THREAD_CS_EXIT(x,y)
#define MPIR_Nest_incr()
#define MPIR_Nest_decr()
#ifdef HAVE_WINDOWS_H
#define MPIU_UNREFERENCED_ARG(a) a
#else
#define MPIU_UNREFERENCED_ARG(a)
#endif
#endif /* ROMIO_INSIDE_MPICH2 */

/* info is a linked list of these structures */
struct MPIR_Info {
    int cookie;
    char *key, *value;
    struct MPIR_Info *next;
};

#define MPIR_INFO_COOKIE 5835657

MPI_Delete_function ADIOI_End_call;

/* common initialization routine */
void MPIR_MPIOInit(int * error_code);


#include "mpioprof.h"

#ifdef MPI_hpux
#  include "mpioinst.h"
#endif /* MPI_hpux */

#endif

