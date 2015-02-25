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
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int jac_save_pid(int pid)
{
    int fd = open(PID_FILEPATH, O_RDWR | O_CREAT,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        return 0;
    }
    if (!lockfile(fd)) {
        close(fd);
        return 0;
    }

    char buf[16];
    snprintf(buf, sizeof(buf) / sizeof(char), "%d", pid);
    return write(fd, buf, sizeof(buf) / sizeof(char));
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
