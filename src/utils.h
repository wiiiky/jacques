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
#ifndef __JAC_UTILS_H__
#define __JAC_UTILS_H__

/*
 * Daemonsize current process
 * Returns current process id on success
 * Returns 0 on error
 */
int jac_daemonize(void);


void jac_close_fds(void);

/*
 * Checks to see if jacques is already running or not
 */
int jac_check_instance(void);


int jac_save_pid(int pid);


void set_proctitle(char **argv,
                   /* argv as passed to main, so args can be moved if necessary */
                   const char *fmt, /* printf(3)-style format string for process title */
                   ... /* args to format string */ );

/*
 * Sets  the  effective  user ID of the calling process.
 */
int set_procuser(const char *username);

#endif
