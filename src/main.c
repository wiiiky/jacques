/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
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

#include "config.h"
#include <jlib/jlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <locale.h>
#include <libintl.h>

#define _(string)   gettext(string)


static struct option long_options[] = {
    {"version", 0, 0, 'v'},
    {"help", 0, 0, 'h'},
    {0, 0, 0, 0},
};

static inline void show_help(void);
static inline void show_version(void);

static inline void start_jacques(void);
static inline void stop_jacques(void);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    int opt;
    while ((opt = getopt_long(argc, argv, "vh", long_options, NULL)) != -1) {
        switch (opt) {
        case 'v':
            show_version();
            break;
        case 'h':
            show_help();
            break;
        default:
            break;
        }
    }
    if (optind != argc - 1) {
        show_help();
    }
    const char *cmd = argv[optind];
    if (j_strcmp0(cmd, "start") == 0) {
        start_jacques();
    } else if (j_strcmp0(cmd, "stop") == 0) {
        stop_jacques();
    }
    printf(_("error: unknown command \'%s\'\n\n"), cmd);
    show_help();

    return 0;
}


static inline void show_help(void)
{
    printf(_("%s version: %s/%s\n"),
           PROGRAME_NAME, PROGRAME_NAME, PROGRAME_VERSION);
    printf(_("Usage: %s start|stop [options]\n\n"), PROGRAME_NAME);
    printf(_("Options:\n"));
    printf(_("  -h\t: show this help\n"));
    printf(_("  -v\t: show version and exit\n"));
    printf("\n");
    exit(0);
}

static inline void show_version(void)
{
    printf(_("%s version: %s/%s\n"),
           PROGRAME_NAME, PROGRAME_NAME, PROGRAME_VERSION);
    exit(0);
}

static inline void start_jacques(void)
{
    printf("started!\n");
    exit(0);
}

static inline void stop_jacques(void)
{
    printf("stopped!\n");
    exit(0);
}
