# configure.ac - MASP Assembly Preprocessor configuration
# Copyright 2003 Johann Gunnar Oskarsson

# Written by Johann Gunnar Oskarsson
#    myrkraverk@users.sourceforge.net

# This file is part of MASP, the Assembly Preprocessor.

# MASP is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# MASP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with MASP; see the file COPYING.  If not, write to the Free
# Software Foundation, 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.


AC_INIT(masp, 0.1.16, myrkraverk@users.sourceforge.net)

AC_CONFIG_SRCDIR([src/masp.c])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PROG_CC(gcc) # There is no support of anything else
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h langinfo.h libintl.h limits.h locale.h stddef.h \
	stdlib.h string.h strings.h sys/file.h sys/param.h sys/time.h \
	unistd.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_STDBOOL
AC_C_INLINE
AC_FUNC_OBSTACK
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_DECLS([sys_siglist])
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])
#AC_CHECK_TYPES([sys_errlist])

# Checks for library functions.
AC_FUNC_VPRINTF
AC_FUNC_CHOWN
AC_REPLACE_FNMATCH
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MBRTOWC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_UTIME_NULL
AC_CHECK_FUNCS([atexit btowc bzero dup2 getcwd getpagesize gettimeofday \
	memchr memmove memset mkdir putenv re_comp regcomp rmdir setenv \
	setlocale strcasecmp strchr strcspn strdup strerror strncasecmp \
	strpbrk strrchr strspn strstr strtol strtoul utime sys_errlist \
	psignal strsignal sysconf])


AC_CONFIG_FILES([Makefile src/Makefile libiberty/Makefile include/Makefile])
#AC_CONFIG_HEADERS([src/config.h libiberty/config.h])
AC_CONFIG_HEADERS([include/config.h])

AC_OUTPUT

### There should be nothing but comments below this line

### Start of src/configure.scan

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ(2.57)
#AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
#AC_CONFIG_SRCDIR([gasp.c])
#AC_CONFIG_HEADER([config.h])

# Checks for programs.
#AC_PROG_CC
#AC_PROG_INSTALL
# Checks for libraries.
# FIXME: Replace `main' with a function in `-liberty':
# AC_CHECK_LIB([iberty], [main])

# Checks for header files.
#AC_FUNC_ALLOCA
#AC_HEADER_STDC
#AC_CHECK_HEADERS([libintl.h locale.h stddef.h stdlib.h string.h strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_C_INLINE
#AC_FUNC_OBSTACK
#AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_CHECK_FUNCS([memset re_comp regcomp setlocale strcspn strdup strncasecmp strstr strtol])

#AC_CONFIG_FILES([Makefile])
#AC_OUTPUT
    

### End of src/configure.scan

### Start of libiberty/configure.scan

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ(2.57)
#AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)
#AC_CONFIG_SRCDIR([_doprnt.c])
#AC_CONFIG_HEADER([config.h])

# Checks for programs.
#AC_PROG_CC
#AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-liberty':
#AC_CHECK_LIB([iberty], [main])

# Checks for header files.
#AC_FUNC_ALLOCA
#AC_HEADER_STDC
#AC_HEADER_SYS_WAIT
#AC_CHECK_HEADERS([fcntl.h langinfo.h libintl.h limits.h stddef.h stdlib.h string.h strings.h sys/file.h sys/param.h sys/time.h unistd.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
dnl dnl AC_C_CONST
dnl dnl AC_HEADER_STDBOOL
dnl dnl AC_C_INLINE
dnl dnl AC_FUNC_OBSTACK
dnl dnl AC_TYPE_PID_T
dnl dnl AC_TYPE_SIZE_T
dnl dnl AC_CHECK_DECLS([sys_siglist])
dnl dnl AC_HEADER_TIME
dnl dnl AC_STRUCT_TM
dnl dnl AC_CHECK_TYPES([ptrdiff_t])

dnl # Checks for library functions.
dnl dnl AC_FUNC_VPRINTF
dnl dnl AC_FUNC_CHOWN
dnl dnl AC_REPLACE_FNMATCH
dnl dnl AC_FUNC_FORK
dnl dnl AC_PROG_GCC_TRADITIONAL
dnl dnl AC_FUNC_MALLOC
dnl dnl AC_FUNC_MBRTOWC
dnl dnl AC_FUNC_MEMCMP
dnl dnl AC_FUNC_REALLOC
dnl dnl AC_FUNC_STAT
dnl dnl AC_FUNC_STRTOD
dnl dnl AC_FUNC_UTIME_NULL
dnl dnl AC_CHECK_FUNCS([atexit btowc bzero dup2 getcwd getpagesize gettimeofday memchr memmove memset mkdir putenv re_comp regcomp rmdir setenv strcasecmp strchr strcspn strdup strerror strncasecmp strpbrk strrchr strspn strstr strtol strtoul utime])

dnl #AC_CONFIG_FILES([Makefile
dnl #                 testsuite/Makefile])
dnl #AC_OUTPUT


dnl ### End of libiberty/configure.scan
		
dnl ### Start of libiberty

dnl dnl dnl Process this file with autoconf to produce a configure script

dnl dnl AC_PREREQ(2.13)
dnl dnl AC_INIT(pexecute.c)

dnl dnl # This works around the fact that libtool configuration may change LD
dnl dnl # for this particular configuration, but some shells, instead of
dnl dnl # keeping the changes in LD private, export them just because LD is
dnl dnl # exported.  We don't use libtool yet, but some day we might, so...
dnl dnl ORIGINAL_LD_FOR_MULTILIBS=$LD

dnl dnl dnl We use these options to decide which functions to include.
dnl dnl AC_ARG_WITH(target-subdir,
dnl dnl [  --with-target-subdir=SUBDIR      Configuring in a subdirectory for target])
dnl dnl AC_ARG_WITH(build-subdir,
dnl dnl [  --with-build-subdir=SUBDIR       Configuring in a subdirectory for build])
dnl dnl AC_ARG_WITH(cross-host,
dnl dnl [  --with-cross-host=HOST           Configuring with a cross compiler])
dnl dnl AC_ARG_WITH(newlib,
dnl dnl [  --with-newlib                    Configuring with newlib])

dnl dnl if test "${srcdir}" = "."; then
dnl dnl   if test -n "${with_build_subdir}"; then
dnl dnl     libiberty_topdir="${srcdir}/../.."
dnl dnl     with_target_subdir=
dnl dnl   elif test -z "${with_target_subdir}"; then
dnl dnl     libiberty_topdir="${srcdir}/.."
dnl dnl   else
dnl dnl     if test "${with_target_subdir}" != "."; then
dnl dnl       libiberty_topdir="${srcdir}/${with_multisrctop}../.."
dnl dnl     else
dnl dnl       libiberty_topdir="${srcdir}/${with_multisrctop}.."
dnl dnl     fi
dnl dnl   fi
dnl dnl else
dnl dnl   libiberty_topdir="${srcdir}/.."
dnl dnl fi
dnl dnl AC_CONFIG_AUX_DIR($libiberty_topdir)

dnl dnl dnl Very limited version of automake's enable-maintainer-mode

dnl dnl AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
dnl dnl   dnl maintainer-mode is disabled by default
dnl dnl   AC_ARG_ENABLE(maintainer-mode,
dnl dnl [  --enable-maintainer-mode
dnl dnl                           enable make rules and dependencies not useful
dnl dnl                           (and sometimes confusing) to the casual installer],
dnl dnl       maintainer_mode=$enableval,
dnl dnl       maintainer_mode=no)

dnl dnl AC_MSG_RESULT($maintainer_mode)

dnl dnl if test "$maintainer_mode" = "yes"; then
dnl dnl   MAINT=''
dnl dnl   NOTMAINT='#'
dnl dnl else
dnl dnl   MAINT='#'
dnl dnl   NOTMAINT=''
dnl dnl fi
dnl dnl AC_SUBST(MAINT)dnl
dnl dnl AC_SUBST(NOTMAINT)dnl

dnl dnl # Do we have a single-tree copy of texinfo?  Even if we do, we can't
dnl dnl # rely on it - libiberty is built before texinfo.
dnl dnl AC_CHECK_PROG(MAKEINFO, makeinfo, makeinfo, )
dnl dnl if test "x$MAKEINFO" = "x"; then
dnl dnl     MAKEINFO="@echo makeinfo missing; true"
dnl dnl     BUILD_INFO=
dnl dnl else
dnl dnl     BUILD_INFO=info
dnl dnl     case x"`$MAKEINFO --version | grep 'GNU texinfo'`" in
dnl dnl       x*\ [[1-3]].* )
dnl dnl 	MAKEINFO="@echo $MAKEINFO is too old, 4.0 or newer required; true"
dnl dnl 	BUILD_INFO=
dnl dnl 	AC_MSG_WARN([
dnl dnl *** Makeinfo is too old. Info documentation will not be built.])
dnl dnl 	;;
dnl dnl     esac
dnl dnl fi
dnl dnl AC_SUBST(MAKEINFO)
dnl dnl AC_SUBST(BUILD_INFO)

dnl dnl AC_CHECK_PROG(PERL, perl, perl, )
dnl dnl if test x"$PERL" = x""; then
dnl dnl   HAVE_PERL='#'
dnl dnl else
dnl dnl   HAVE_PERL=''
dnl dnl fi
dnl dnl AC_SUBST(HAVE_PERL)

dnl dnl AC_CANONICAL_HOST

dnl dnl dnl When we start using automake:
dnl dnl dnl AM_INIT_AUTOMAKE(libiberty, 1.0)

dnl dnl dnl These must be called before AM_PROG_LIBTOOL, because it may want
dnl dnl dnl to call AC_CHECK_PROG.
    
dnl AC_CHECK_TOOL(AR, ar)
dnl AC_CHECK_TOOL(RANLIB, ranlib, :)

dnl dnl LIB_AC_PROG_CC

dnl dnl AC_PROG_CC_C_O
dnl dnl # autoconf is lame and doesn't give us any substitution variable for this.
dnl dnl if eval "test \"`echo '$ac_cv_prog_cc_'${ac_cc}_c_o`\" = no"; then
dnl dnl   NO_MINUS_C_MINUS_O=yes
dnl dnl else
dnl dnl   OUTPUT_OPTION='-o $@'
dnl dnl fi
dnl dnl AC_SUBST(NO_MINUS_C_MINUS_O)
dnl dnl AC_SUBST(OUTPUT_OPTION)

dnl dnl AC_ISC_POSIX
dnl dnl AC_C_CONST
dnl dnl AC_C_INLINE

dnl dnl dnl When we start using libtool:
dnl dnl dnl Default to a non shared library.  This may be overridden by the
dnl dnl dnl configure option --enable-shared.
dnl dnl dnl AM_DISABLE_SHARED

dnl dnl dnl When we start using libtool:
dnl dnl dnl AM_PROG_LIBTOOL

dnl dnl dnl When we start using automake:
dnl dnl dnl AM_CONFIG_HEADER(config.h:config.in)
dnl dnl AC_CONFIG_HEADER(libiberty/config.h:libiberty/config.in)

dnl dnl dnl When we start using automake:
dnl dnl dnl AM_MAINTAINER_MODE
dnl dnl dnl AC_EXEEXT

dnl dnl dnl When we start using automake:
dnl dnl dnl AM_PROG_INSTALL
dnl dnl AC_PROG_INSTALL

dnl dnl . ${srcdir}/config.table
dnl dnl host_makefile_frag=${frag}
dnl dnl AC_SUBST_FILE(host_makefile_frag)

dnl dnl # It's OK to check for header files.  Although the compiler may not be
dnl dnl # able to link anything, it had better be able to at least compile
dnl dnl # something.
dnl AC_CHECK_HEADERS(sys/file.h sys/param.h limits.h stdlib.h string.h unistd.h strings.h sys/time.h time.h sys/resource.h sys/stat.h sys/mman.h fcntl.h alloca.h)
dnl dnl AC_HEADER_SYS_WAIT
dnl dnl AC_HEADER_TIME

dnl dnl libiberty_AC_DECLARE_ERRNO

dnl AC_CHECK_TYPE(uintptr_t, unsigned long)

dnl dnl if test $ac_cv_type_uintptr_t = yes
dnl dnl then
dnl dnl   AC_DEFINE(HAVE_UINTPTR_T, 1, [Define if you have the \`uintptr_t' type.])
dnl dnl fi

dnl AC_TYPE_PID_T

dnl dnl # This is the list of functions which libiberty will provide if they
dnl dnl # are not available on the host.

dnl dnl funcs="asprintf"
dnl dnl funcs="$funcs atexit"
dnl dnl funcs="$funcs basename"
dnl dnl funcs="$funcs bcmp"
dnl dnl funcs="$funcs bcopy"
dnl dnl funcs="$funcs bsearch"
dnl dnl funcs="$funcs bzero"
dnl dnl funcs="$funcs calloc"
dnl dnl funcs="$funcs clock"
dnl dnl funcs="$funcs ffs"
dnl dnl funcs="$funcs getcwd"
dnl dnl funcs="$funcs getpagesize"
dnl dnl funcs="$funcs index"
dnl dnl funcs="$funcs insque"
dnl dnl funcs="$funcs memchr"
dnl dnl funcs="$funcs memcmp"
dnl dnl funcs="$funcs memcpy"
dnl dnl funcs="$funcs memmove"
dnl dnl funcs="$funcs memset"
dnl dnl funcs="$funcs mkstemps"
dnl dnl funcs="$funcs putenv"
dnl dnl funcs="$funcs random"
dnl dnl funcs="$funcs rename"
dnl dnl funcs="$funcs rindex"
dnl dnl funcs="$funcs setenv"
dnl dnl funcs="$funcs sigsetmask"
dnl dnl funcs="$funcs strcasecmp"
dnl dnl funcs="$funcs strchr"
dnl dnl funcs="$funcs strdup"
dnl dnl funcs="$funcs strncasecmp"
dnl dnl funcs="$funcs strrchr"
dnl dnl funcs="$funcs strstr"
dnl dnl funcs="$funcs strtod"
dnl dnl funcs="$funcs strtol"
dnl dnl funcs="$funcs strtoul"
dnl dnl funcs="$funcs tmpnam"
dnl dnl funcs="$funcs vasprintf"
dnl dnl funcs="$funcs vfprintf"
dnl dnl funcs="$funcs vprintf"
dnl dnl funcs="$funcs vsprintf"
dnl dnl funcs="$funcs waitpid"

dnl dnl # Also in the old function.def file: alloca, vfork, getopt.

dnl dnl vars="sys_errlist sys_nerr sys_siglist"

dnl dnl checkfuncs="getrusage on_exit psignal strerror strsignal sysconf times sbrk gettimeofday"

dnl dnl # These are neither executed nor required, but they help keep
dnl dnl # autoheader happy without adding a bunch of text to acconfig.h.
dnl dnl if test "x" = "y"; then
dnl dnl   AC_CHECK_FUNCS(asprintf atexit basename bcmp bcopy bsearch bzero calloc clock)
dnl dnl   AC_CHECK_FUNCS(getcwd getpagesize index insque mkstemps memchr memcmp memcpy)
dnl dnl   AC_CHECK_FUNCS(memmove memset putenv random rename rindex sigsetmask)
dnl dnl   AC_CHECK_FUNCS(strcasecmp setenv strchr strdup strncasecmp strrchr strstr)
dnl dnl   AC_CHECK_FUNCS(strtod strtol strtoul tmpnam vasprintf vfprintf vprintf)
dnl dnl   AC_CHECK_FUNCS(vsprintf waitpid getrusage on_exit psignal strerror strsignal)
dnl dnl   AC_CHECK_FUNCS(sysconf times sbrk gettimeofday ffs)
dnl dnl   AC_DEFINE(HAVE_SYS_ERRLIST, 1, [Define if you have the sys_errlist variable.])
dnl dnl   AC_DEFINE(HAVE_SYS_NERR,    1, [Define if you have the sys_nerr variable.])
dnl dnl   AC_DEFINE(HAVE_SYS_SIGLIST, 1, [Define if you have the sys_siglist variable.])
dnl dnl fi

dnl dnl # For each of these functions, if the host does not provide the
dnl dnl # function we want to put FN.o in LIBOBJS, and if the host does
dnl dnl # provide the function, we want to define HAVE_FN in config.h.

dnl dnl setobjs=
dnl dnl CHECK=
dnl dnl target_header_dir=
dnl dnl if test -n "${with_target_subdir}"; then

dnl dnl   # We are being configured as a target library.  AC_REPLACE_FUNCS
dnl dnl   # may not work correctly, because the compiler may not be able to
dnl dnl   # link executables.  Note that we may still be being configured
dnl dnl   # native.

dnl dnl   # If we are being configured for newlib, we know which functions
dnl dnl   # newlib provide and which ones we will be expected to provide.

dnl dnl   if test "x${with_newlib}" = "xyes"; then
dnl dnl     LIBOBJS="asprintf.o basename.o insque.o random.o strdup.o vasprintf.o"

dnl dnl     for f in $funcs; do
dnl dnl       case "$f" in
dnl dnl 	asprintf | basename | insque | random | strdup | vasprintf)
dnl dnl 	  ;;
dnl dnl 	*)
dnl dnl 	  n=HAVE_`echo $f | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`
dnl dnl 	  AC_DEFINE_UNQUOTED($n)
dnl dnl 	  ;;
dnl dnl       esac
dnl dnl     done

dnl dnl     # newlib doesnt provide any of the variables in $vars, so we
dnl dnl     # dont have to check them here.

dnl dnl     # Of the functions in $checkfuncs, newlib only has strerror.
dnl dnl     AC_DEFINE_NOAUTOHEADER(HAVE_STRERROR)

dnl dnl     setobjs=yes

dnl dnl   fi

dnl dnl   # We may wish to install the target headers somewhere.
dnl dnl   AC_ARG_ENABLE(install-libiberty,
dnl dnl   [  --enable-install-libiberty       Install headers for end users],
dnl dnl   enable_install_libiberty=$enableval,
dnl dnl   enable_install_libiberty=no)dnl
  
dnl dnl   # Option parsed, now set things appropriately.
dnl dnl   case x"$enable_install_libiberty" in
dnl dnl     xyes|x)
dnl dnl       target_header_dir=libiberty
dnl dnl       ;;
dnl dnl     xno)   
dnl dnl       target_header_dir=
dnl dnl       ;;
dnl dnl     *) 
dnl dnl       # This could be sanity-checked in various ways...
dnl dnl       target_header_dir="${enable_install_libiberty}"
dnl dnl       ;;
dnl dnl   esac


dnl dnl else

dnl dnl    # Not a target library, so we set things up to run the test suite.
dnl dnl    CHECK=check-cplus-dem

dnl dnl fi

dnl dnl AC_SUBST(CHECK)
dnl dnl AC_SUBST(target_header_dir)

dnl dnl case "${host}" in
dnl dnl   *-*-cygwin* | *-*-mingw*)
dnl dnl     AC_DEFINE_NOAUTOHEADER(HAVE_SYS_ERRLIST)
dnl dnl     AC_DEFINE_NOAUTOHEADER(HAVE_SYS_NERR)
dnl dnl     ;;
dnl dnl esac

dnl dnl if test -z "${setobjs}"; then
dnl dnl   case "${host}" in

dnl dnl   *-*-vxworks*)
dnl dnl     # Handle VxWorks configuration specially, since on VxWorks the
dnl dnl     # libraries are actually on the target board, not in the file
dnl dnl     # system.
dnl dnl     LIBOBJS="basename.o getpagesize.o insque.o random.o strcasecmp.o"
dnl dnl     LIBOBJS="$LIBOBJS strncasecmp.o strdup.o vfork.o waitpid.o vasprintf.o"
dnl dnl     for f in $funcs; do
dnl dnl       case "$f" in
dnl dnl 	basename | getpagesize | insque | random | strcasecmp)
dnl dnl 	  ;;
dnl dnl 	strncasecmp | strdup | vfork | waitpid | vasprintf)
dnl dnl 	  ;;
dnl dnl 	*)
dnl dnl 	  n=HAVE_`echo $f | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`
dnl dnl 	  AC_DEFINE_UNQUOTED($n)
dnl dnl 	  ;;
dnl dnl       esac
dnl dnl     done

dnl dnl     # VxWorks doesn't provide any of the variables in $vars, so we
dnl dnl     # don't have to check them here.

dnl dnl     # Of the functions in $checkfuncs, VxWorks only has strerror.
dnl dnl     AC_DEFINE_NOAUTOHEADER(HAVE_STRERROR)

dnl dnl     setobjs=yes
dnl dnl     ;;

dnl dnl   esac
dnl dnl fi

dnl dnl if test -z "${setobjs}"; then

dnl dnl   case "${host}" in

dnl dnl   *-*-cygwin*)
dnl dnl     # The Cygwin library actually uses a couple of files from
dnl dnl     # libiberty when it is built.  If we are building a native
dnl dnl     # Cygwin, and we run the tests, we will appear to have these
dnl dnl     # files.  However, when we go on to build winsup, we will wind up
dnl dnl     # with a library which does not have the files, since they should
dnl dnl     # have come from libiberty.

dnl dnl     # We handle this by removing the functions the winsup library
dnl dnl     # provides from our shell variables, so that they appear to be
dnl dnl     # missing.

dnl dnl     # DJ - only if we're *building* cygwin, not just building *with* cygwin
  
dnl dnl     if test -n "${with_target_subdir}"
dnl dnl     then
dnl dnl       funcs="`echo $funcs | sed -e 's/random//'`"
dnl dnl       LIBOBJS="$LIBOBJS random.o"
dnl dnl       vars="`echo $vars | sed -e 's/sys_siglist//'`"
dnl dnl       checkfuncs="`echo $checkfuncs | sed -e 's/strsignal//' -e 's/psignal//'`"
dnl dnl     fi
dnl dnl     ;;

dnl dnl   *-*-mingw32*)
dnl dnl     # Under mingw32, sys_nerr and sys_errlist exist, but they are
dnl dnl     # macros, so the test below won't find them.
dnl dnl     libiberty_cv_var_sys_nerr=yes
dnl dnl     libiberty_cv_var_sys_errlist=yes
dnl dnl     ;;

dnl dnl   *-*-uwin*)
dnl dnl     # Under some versions of uwin, vfork is notoriously buggy and the test 
dnl dnl     # can hang configure; on other versions, vfork exists just as a stub.
dnl dnl     # FIXME: This should be removed once vfork in uwin's runtime is fixed.
dnl dnl     ac_cv_func_vfork_works=no
dnl dnl     # Under uwin 2.0+, sys_nerr and sys_errlist exist, but they are
dnl dnl     # macros (actually, these are imported from a DLL, but the end effect 
dnl dnl     # is the same), so the test below won't find them.
dnl dnl     libiberty_cv_var_sys_nerr=yes
dnl dnl     libiberty_cv_var_sys_errlist=yes
dnl dnl     ;;

dnl dnl   *-*-*vms*)
dnl dnl     # Under VMS, vfork works very different than on Unix. The standard test 
dnl dnl     # won't work, and it isn't easily adaptable. It makes more sense to
dnl dnl     # just force it.
dnl dnl     ac_cv_func_vfork_works=yes
dnl dnl     ;;

dnl dnl   esac

dnl dnl   # We haven't set the list of objects yet.  Use the standard autoconf
dnl dnl   # tests.  This will only work if the compiler works.
dnl dnl   AC_PROG_CC_WORKS
dnl dnl   AC_REPLACE_FUNCS($funcs)
dnl dnl   libiberty_AC_FUNC_C_ALLOCA
dnl dnl   AC_FUNC_VFORK
dnl dnl   if test $ac_cv_func_vfork_works = no; then
dnl dnl     LIBOBJS="$LIBOBJS vfork.o"
dnl dnl   fi
dnl dnl   # We only need _doprnt if we might use it to implement v*printf.
dnl dnl   if test $ac_cv_func_vprintf != yes \
dnl dnl      || test $ac_cv_func_vfprintf != yes \
dnl dnl      || test $ac_cv_func_vsprintf != yes; then
dnl dnl     AC_REPLACE_FUNCS(_doprnt)
dnl dnl   else
dnl dnl     AC_CHECK_FUNCS(_doprnt)
dnl dnl   fi

dnl dnl   for v in $vars; do
dnl dnl     AC_MSG_CHECKING([for $v])
dnl dnl     AC_CACHE_VAL(libiberty_cv_var_$v,
dnl dnl       [AC_TRY_LINK([int *p;], [extern int $v []; p = $v;],
dnl dnl 		   [eval "libiberty_cv_var_$v=yes"],
dnl dnl 		   [eval "libiberty_cv_var_$v=no"])])
dnl dnl     if eval "test \"`echo '$libiberty_cv_var_'$v`\" = yes"; then
dnl dnl       AC_MSG_RESULT(yes)
dnl dnl       n=HAVE_`echo $v | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`
dnl dnl       AC_DEFINE_UNQUOTED($n)
dnl dnl     else
dnl dnl       AC_MSG_RESULT(no)
dnl dnl     fi
dnl dnl   done
dnl dnl   AC_CHECK_FUNCS($checkfuncs)
dnl dnl fi

dnl dnl libiberty_AC_FUNC_STRNCMP

dnl dnl # Install a library built with a cross compiler in $(tooldir) rather
dnl dnl # than $(libdir).
dnl dnl if test -z "${with_cross_host}"; then
dnl dnl   INSTALL_DEST=libdir
dnl dnl else
dnl dnl   INSTALL_DEST=tooldir
dnl dnl fi
dnl dnl AC_SUBST(INSTALL_DEST)

dnl dnl # We need multilib support, but only if configuring for the target.
dnl dnl AC_OUTPUT(Makefile testsuite/Makefile,
dnl dnl [test -z "$CONFIG_HEADERS" || echo timestamp > stamp-h
dnl dnl if test -n "$CONFIG_FILES"; then
dnl dnl   if test -n "${with_build_subdir}" || test -n "${with_target_subdir}"; then
dnl dnl     # FIXME: We shouldn't need to set ac_file
dnl dnl     ac_file=Makefile
dnl dnl     LD="${ORIGINAL_LD_FOR_MULTILIBS}"
dnl dnl     . ${libiberty_topdir}/config-ml.in
dnl dnl   fi
dnl dnl fi],
dnl dnl srcdir=${srcdir}
dnl dnl host=${host}
dnl dnl target=${target}
dnl dnl with_target_subdir=${with_target_subdir}
dnl dnl with_build_subdir=${with_build_subdir}
dnl dnl with_multisubdir=${with_multisubdir}
dnl dnl ac_configure_args="--enable-multilib ${ac_configure_args}"
dnl dnl CONFIG_SHELL=${CONFIG_SHELL-/bin/sh}
dnl dnl libiberty_topdir=${libiberty_topdir}
dnl dnl )


### End of libiberty

#AC_OUTPUT