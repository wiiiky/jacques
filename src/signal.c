/*
 * Copyright (C) 2015 Wiky L
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#include "signal_.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


static ev_signal signal_watcher;

static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents) {
    ev_break (loop, EVBREAK_ALL);

    ev_signal_stop(loop, w);

    printf("SIGINT!\n");
}

/* 初始化信号处理函数 */
void signal_init(struct ev_loop *loop) {
    ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
    ev_signal_start (loop, &signal_watcher);
}
