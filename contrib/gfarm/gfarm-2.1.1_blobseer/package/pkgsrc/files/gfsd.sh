#!@RCD_SCRIPTS_SHELL@
#
# $NetBSD$
#
# Gfarm filesystem node daemon
#
# PROVIDE: gfsd
# REQUIRE: DAEMON

if [ -f /etc/rc.subr ]; then
	. /etc/rc.subr
fi

name="gfsd"
FILE=${name}
rcvar=${name}
command="@PREFIX@/sbin/${name}"
pidfile="@GFARM_PID_DIR@/${FILE}.pid"
required_files="@PKG_SYSCONFDIR@/gfarm.conf"
# add more flags through ${${name}_flags}
command_args="-P $pidfile"

if [ -f /etc/rc.subr ]; then
	load_rc_config ${FILE}
	run_rc_command "$1"
else
	@ECHO@ -n " ${name}"
	${command} ${gfsd_flags} ${command_args}
fi
