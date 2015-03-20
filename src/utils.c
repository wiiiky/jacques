/*
 * Copyright (C) 2015 Wiky L <wiiiky@outlook.com>
 *
 * jacques is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jacques is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#include "utils.h"
#include "config.h"
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <pwd.h>
#include <grp.h>


/*
 * Locks a file
 */
static inline int lockfile(int fd);

/*
 * Daemonsize current process
 * Returns current process id on success
 * Returns 0 on error
 */
int jac_daemonize(void)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    /*
     * Clears file creation mask
     */
    umask(0);
    /*
     * Gets maximum number of file descriptor
     */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        return 0;
    }
    /*
     * Becomes a session leader to lose controlling TTY
     */
    if ((pid = fork()) < 0) {
        return 0;
    } else if (pid != 0) {
        exit(0);
    }
    setsid();

    /*
     * Ensure future opens won't allocate controlling TTYs
     */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        return 0;
    }
    if ((pid = fork()) < 0) {
        return 0;
    } else if (pid > 0) {
        exit(0);
    }

    /*
     * Change the current working directory to the root so we won't
     * prevent file systems from being unmounted
     */
    if (chdir("/") < 0) {
        return 0;
    }

    /*
     * Close all open file descriptor
     */
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    /*
     * Attach file descriptors 0, 1 and 2 to /dev/null
     */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(fd0);
    fd2 = dup(fd0);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        return 0;
    }

    return (int) getpid();
}

void jac_close_fds(void)
{
    int i;
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        return;
    }
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (i = 3; i < rl.rlim_max; i++) {
        close(i);
    }
}

/*
 * Checks to see if jacques is already running or not
 */
int jac_check_instance(void)
{
    int fd = open(PID_FILEPATH, O_RDWR | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        return -1;
    }
    if (!lockfile(fd)) {
        char buf[32];
        int n = read(fd, buf, sizeof(buf) / sizeof(char));
        close(fd);
        if (n > 0) {
            buf[n] = '\0';
            int pid = atoi(buf);
            if (pid > 0) {
                return pid;
            }
            return 0;
        }
        return 1;
    }
    close(fd);
    return 0;
}

int jac_save_pid(int pid)
{
    int fd = open(PID_FILEPATH, O_RDWR | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        return 0;
    }
    if (!lockfile(fd)) {
        close(fd);
        return 0;
    }

    char buf[16];
    int len = snprintf(buf, sizeof(buf) / sizeof(char), "%d", pid);
    return write(fd, buf, len);
}

static inline int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return !fcntl(fd, F_SETLK, &fl);
}

extern char **environ;
static char *argstart = NULL;
static size_t maxarglen;        /* maximum available size of argument area */
static int envmoved = 0;

void set_proctitle(char **argv,
                   /* argv as passed to main, so args can be moved if necessary */
                   const char *fmt, /* printf(3)-style format string for process title */
                   ... /* args to format string */ )
  /* something as close as possible to BSD setproctitle(3), but for Linux.
     Note I need argv as passed to main, in order to be able to poke the process
     arguments area. Also don't call routines like putenv(3) or setenv(3)
     prior to using this routine. */
{
    char title[512];            /* big enough? */
    ssize_t titlelen;
    {
        va_list args;
        va_start(args, fmt);
        titlelen = vsnprintf(title, sizeof title, fmt, args);
        va_end(args);
        if (titlelen < 0) {
            titlelen = 0;       /* ignore error */
            title[0] = 0;
        }                       /*if */
        titlelen += 1;          /* including trailing nul */
        if (titlelen > sizeof title) {
            title[sizeof title - 1] = '\0'; /* do I need to do this? */
            titlelen = sizeof title;
        }                       /*if */
    }
    if (argstart == NULL) {
        /* first call, find and initialize argument area */
        char **thisarg = argv;
        maxarglen = 0;
        argstart = *thisarg;
        while (*thisarg != NULL) {
            maxarglen += strlen(*thisarg++) + 1;    /* including terminating nul */
        }                       /*while */
        memset(argstart, 0, maxarglen); /* clear it all out */
    }                           /*if */
    if (titlelen > maxarglen && !envmoved) {
        /* relocate the environment strings and use that area for the command line
           as well */
        char **srcenv;
        char **dstenv;
        char **newenv;
        size_t envlen = 0;
        size_t nrenv = 1;       /* nr env strings + 1 for terminating NULL pointer */
        if (argstart + maxarglen == environ[0]) {   /* not already moved by e.g. libc */
            srcenv = environ;
            while (*srcenv != NULL) {
                envlen += strlen(*srcenv++) + 1;    /* including terminating nul */
                ++nrenv;        /* count 'em up */
            }                   /*while */
            newenv = (char **) malloc(sizeof(char *) * nrenv);  /* new env array, never freed! */
            srcenv = environ;
            dstenv = newenv;
            while (*srcenv != NULL) {
                /* copy the environment strings */
                *dstenv++ = strdup(*srcenv++);
            }                   /*while */
            *dstenv = NULL;     /* mark end of new environment array */
            memset(environ[0], 0, envlen);  /* clear old environment area */
            maxarglen += envlen;    /* this much extra space now available */
            environ = newenv;   /* so libc etc pick up new environment location */
        }                       /*if */
        envmoved = 1;
    }                           /*if */
    if (titlelen > maxarglen) {
        titlelen = maxarglen;   /* truncate to fit available area */
    }                           /*if */
    if (titlelen > 0) {
        /* set the new title */
        const size_t oldtitlelen = strlen(argstart) + 1;    /* including trailing nul */
        memcpy(argstart, title, titlelen);
        argstart[titlelen - 1] = '\0';  /* if not already done */
        if (oldtitlelen > titlelen) {
            /* wipe out remnants of previous title */
            memset(argstart + titlelen, 0, oldtitlelen - titlelen);
        }                       /*if */
    }                           /*if */
}                               /*setproctitle */



/*
 * Sets  the  effective  user ID of the calling process.
 */
int set_procuser(const char *username, const char *groupname)
{
    struct passwd *pwd = getpwnam(username);
    struct group *grp = getgrnam(groupname);

    if (pwd == NULL || grp == NULL) {
        return 0;
    }
    setgid(grp->gr_gid);
    setegid(grp->gr_gid);

    setuid(pwd->pw_uid);
    seteuid(pwd->pw_uid);
    return 1;
}
