prefix = /Users/vitran/irisawork/Gfarm_blobSeer/gfarm_bin/
exec_prefix = ${prefix}

# save default for reference from where installation directory is overridden.
default_sbindir = ${exec_prefix}/sbin
default_bindir = ${exec_prefix}/bin
default_libdir = ${exec_prefix}/lib
default_libexecdir = ${exec_prefix}/libexec
default_includedir = ${prefix}/include
default_exec_includedir = ${prefix}/include
default_datadir = ${prefix}/share
default_mandir = ${prefix}/share/man
default_docdir = $(default_datadir)/doc/gfarm
default_htmldir = $(default_docdir)/html
default_sysconfdir= ${prefix}/etc

sbindir = $(default_sbindir)
bindir = $(default_bindir)
libdir = $(default_libdir)
libexecdir = $(default_libexecdir)
includedir = $(default_includedir)
exec_includedir = $(default_exec_includedir)
datadir = $(default_datadir)
mandir = $(default_mandir)
docdir = $(default_docdir)
htmldir = $(default_htmldir)
sysconfdir = $(default_sysconfdir)

CC = cc
OPTFLAGS = -g -O2 -Wall
LEX = flex
LEXLIB = -lfl
YACC = bison -y
RANLIB = ranlib
LIBTOOL = $(SHELL) $(top_builddir)/libtool
# the use of the following operations should honor $(DESTDIR)
MKDIR_P = mkdir -p
# We don't use "/usr/bin/install -c" here, since we want absolute pathname 
# instead of relative pathname for the case that install-sh is selected.
# XXX this isn't efficient on platforms which have working /usr/bin/install.
INSTALL = $(top_srcdir)/makes/install-sh -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_DOC = sh $(top_builddir)/makes/install-doc

LIBS = 

largefile_cflags = 
largefile_ldflags = 
# `getconf LFS_LIBS` is already added to $(LIBS)

# for conditional compilation which depends on _LARGEFILE64_SOURCE API
largefile64_srcs = 
largefile64_objs = 
largefile64_cflags = 
largefile64_targets = 

pthread_includes = -D_REENTRANT -pthread
pthread_libs =  -pthread
# for conditional compilation which depends on whether pthread is enabled or not
pthread_srcs = ${PTHREAD_SRCS}
pthread_objs = ${PTHREAD_OBJS}
pthread_cflags = ${PTHREAD_CFLAGS}
pthread_targets = ${PTHREAD_TARGETS}

openssl_includes = 
openssl_libs = -lssl -lcrypto

BlobSeer_includes = 
BlobSeer_libs = -lblobseer-c


globus_flavor = 
globus_location = 
globus_includes = 
globus_gssapi_libs = 
# for conditional compilation which depends on whether globus is enabled or not
globus_srcs = 
globus_objs = 
globus_cflags = 
globus_targets = 

# for conditional compilation related to syscall hooking library
gfshook_srcs = ${GFSHOOK_SRCS}
gfshook_objs = ${GFSHOOK_OBJS}
gfshook_cflags = ${GFSHOOK_CFLAGS}
gfshook_targets = ${GFSHOOK_TARGETS}

MPICC = 
# for conditional compilation which depends on whether MPI is enabled or not
mpi_srcs = 
mpi_objs = 
mpi_cflags = 
mpi_targets = 

readline_includes = 
readline_libs = 

ldap_includes = 
ldap_libs =  -lldap -llber
# for conditional compilation which depends on whether LDAP is enabled or not
ldap_srcs = ${LDAP_SRCS}
ldap_objs = ${LDAP_OBJS}
ldap_cflags = ${LDAP_CFLAGS}
ldap_targets = ${LDAP_TARGETS}

postgresql_includes = -I/Users/vitran/irisawork/Gfarm_blobSeer/postgres_bin/include
postgresql_libs = -R/Users/vitran/irisawork/Gfarm_blobSeer/postgres_bin/lib -L/Users/vitran/irisawork/Gfarm_blobSeer/postgres_bin/lib -lpq
# for conditional compilation which depends on whether PostgreSQL is enabled or not
postgresql_srcs = ${POSTGRESQL_SRCS}
postgresql_objs = ${POSTGRESQL_OBJS}
postgresql_cflags = ${POSTGRESQL_CFLAGS}
postgresql_targets = ${POSTGRESQL_TARGETS}

# build lib/gfs_hook/sysdep/$(gfs_hook_sysdep_subdir)
gfs_hook_sysdep_subdir = default

# build gftool/config-gfarm/sysdep/$(config_gfarm_sysdep_subdir)
config_gfarm_sysdep_subdir = default
