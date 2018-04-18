GETCWD(3)                                                                Linux Programmer's Manual                                                               GETCWD(3)

NNAAMMEE
       getcwd, getwd, get_current_dir_name - get current working directory

SSYYNNOOPPSSIISS
       ##iinncclluuddee <<uunniissttdd..hh>>

       cchhaarr **ggeettccwwdd((cchhaarr **_b_u_f,, ssiizzee__tt _s_i_z_e));;

       cchhaarr **ggeettwwdd((cchhaarr **_b_u_f));;

       cchhaarr **ggeett__ccuurrrreenntt__ddiirr__nnaammee((vvooiidd));;

   Feature Test Macro Requirements for glibc (see ffeeaattuurree__tteesstt__mmaaccrrooss(7)):

       ggeett__ccuurrrreenntt__ddiirr__nnaammee():
              _GNU_SOURCE

       ggeettwwdd():
           Since glibc 2.12:
               _BSD_SOURCE ||
                   (_XOPEN_SOURCE >= 500 ||
                       _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) &&
                   !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
           Before glibc 2.12:
               _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED

DDEESSCCRRIIPPTTIIOONN
       These  functions  return  a  null-terminated  string containing an absolute pathname that is the current working directory of the calling process.  The pathname is
       returned as the function result and via the argument _b_u_f, if present.

       If the current directory is not below the root directory of the current process (e.g., because the process set a new filesystem root using cchhrroooott(2) without chang‐
       ing its current directory into the new root), then, since Linux 2.6.36, the returned path will be prefixed with the string "(unreachable)".  Such behavior can also
       be caused by an unprivileged user by changing the current directory into another mount namespace.  When dealing with paths from untrusted sources, callers of these
       functions should consider checking whether the returned path starts with '/' or '(' to avoid misinterpreting an unreachable path as a relative path.

       The ggeettccwwdd() function copies an absolute pathname of the current working directory to the array pointed to by _b_u_f, which is of length _s_i_z_e.

       If  the  length  of the absolute pathname of the current working directory, including the terminating null byte, exceeds _s_i_z_e bytes, NULL is returned, and _e_r_r_n_o is
       set to EERRAANNGGEE; an application should check for this error, and allocate a larger buffer if necessary.

       As an extension to the POSIX.1-2001 standard, glibc's ggeettccwwdd() allocates the buffer dynamically using mmaalllloocc(3) if _b_u_f is NULL.  In this case, the allocated buffer
       has the length _s_i_z_e unless _s_i_z_e is zero, when _b_u_f is allocated as big as necessary.  The caller should ffrreeee(3) the returned buffer.

       ggeett__ccuurrrreenntt__ddiirr__nnaammee()  will mmaalllloocc(3) an array big enough to hold the absolute pathname of the current working directory.  If the environment variable PPWWDD is set,
       and its value is correct, then that value will be returned.  The caller should ffrreeee(3) the returned buffer.

       ggeettwwdd() does not mmaalllloocc(3) any memory.  The _b_u_f argument should be a pointer to an array at least PPAATTHH__MMAAXX bytes long.  If the length of the absolute  pathname  of
       the current working directory, including the terminating null byte, exceeds PPAATTHH__MMAAXX bytes, NULL is returned, and _e_r_r_n_o is set to EENNAAMMEETTOOOOLLOONNGG.  (Note that on some
       systems, PPAATTHH__MMAAXX may not be a compile-time constant; furthermore, its value may depend on the filesystem, see ppaatthhccoonnff(3).)  For portability and security reasons,
       use of ggeettwwdd() is deprecated.

RREETTUURRNN VVAALLUUEE
       On  success,  these  functions return a pointer to a string containing the pathname of the current working directory.  In the case ggeettccwwdd() and ggeettwwdd() this is the
       same value as _b_u_f.

       On failure, these functions return NULL, and _e_r_r_n_o is set to indicate the error.  The contents of the array pointed to by _b_u_f are undefined on error.

EERRRROORRSS
       EEAACCCCEESS Permission to read or search a component of the filename was denied.

       EEFFAAUULLTT _b_u_f points to a bad address.

       EEIINNVVAALL The _s_i_z_e argument is zero and _b_u_f is not a null pointer.

       EEIINNVVAALL ggeettwwdd(): _b_u_f is NULL.

       EENNAAMMEETTOOOOLLOONNGG
              ggeettwwdd(): The size of the null-terminated absolute pathname string exceeds PPAATTHH__MMAAXX bytes.

       EENNOOMMEEMM Out of memory.

       EENNOOEENNTT The current working directory has been unlinked.

       EERRAANNGGEE The _s_i_z_e argument is less than the length of the absolute pathname of the working directory, including the terminating null byte.  You need  to  allocate  a
              bigger array and try again.

AATTTTRRIIBBUUTTEESS
       For an explanation of the terms used in this section, see aattttrriibbuutteess(7).

       ┌───────────────────────┬───────────────┬─────────────┐
       │IInntteerrffaaccee              │ AAttttrriibbuuttee     │ VVaalluuee       │
       ├───────────────────────┼───────────────┼─────────────┤
       │ggeettccwwdd(), ggeettwwdd()      │ Thread safety │ MT-Safe     │
       ├───────────────────────┼───────────────┼─────────────┤
       │ggeett__ccuurrrreenntt__ddiirr__nnaammee() │ Thread safety │ MT-Safe env │
       └───────────────────────┴───────────────┴─────────────┘
CCOONNFFOORRMMIINNGG TTOO
       ggeettccwwdd() conforms to POSIX.1-2001.  Note however that POSIX.1-2001 leaves the behavior of ggeettccwwdd() unspecified if _b_u_f is NULL.

       ggeettwwdd() is present in POSIX.1-2001, but marked LEGACY.  POSIX.1-2008 removes the specification of ggeettwwdd().  Use ggeettccwwdd() instead.  POSIX.1-2001 does not define any
       errors for ggeettwwdd().

       ggeett__ccuurrrreenntt__ddiirr__nnaammee() is a GNU extension.

NNOOTTEESS
       Under Linux, the function ggeettccwwdd() is a system call (since 2.1.92).  On older systems it would query _/_p_r_o_c_/_s_e_l_f_/_c_w_d.  If both system call and proc  filesystem  are
       missing, a generic implementation is called.  Only in that case can these calls fail under Linux with EEAACCCCEESS.

       These functions are often used to save the location of the current working directory for the purpose of returning to it later.  Opening the current directory (".")
       and calling ffcchhddiirr(2) to return is usually a faster and more reliable alternative when sufficiently many file descriptors are available,  especially  on  platforms
       other than Linux.

SSEEEE AALLSSOO
       cchhddiirr(2), ffcchhddiirr(2), ooppeenn(2), uunnlliinnkk(2), ffrreeee(3), mmaalllloocc(3)

CCOOLLOOPPHHOONN
       This  page  is part of release 4.04 of the Linux _m_a_n_-_p_a_g_e_s project.  A description of the project, information about reporting bugs, and the latest version of this
       page, can be found at http://www.kernel.org/doc/man-pages/.

GNU                                                                             2015-04-19                                                                       GETCWD(3)
