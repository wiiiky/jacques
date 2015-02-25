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
#include "server.h"
#include "module.h"
#include "i18n.h"
#include "utils.h"
#include <jlib/jlib.h>
#include <jio/jio.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


static struct option long_options[] = {
    {"version", 0, 0, 'v'},
    {"help", 0, 0, 'h'},
    {0, 0, 0, 0},
};

static inline void show_help(void);
static inline void show_version(void);



static inline JConfParser *initialize_jacques(void);

/*
 * Commands handler
 */
static inline void start_jacques(void);
static inline void stop_jacques(void);
static inline void check_jacques(void);

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
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
    } else if (j_strcmp0(cmd, "check") == 0) {
        check_jacques();
    }
    printf(_("error: unknown command \'%s\'\n\n"), cmd);
    show_help();

    return 0;
}


static inline void show_help(void)
{
    printf(_("%s version: %s/%s\n"),
           PROGRAME_NAME, PROGRAME_NAME, PROGRAME_VERSION);
    printf(_("Usage: %s start|stop|check [options]\n\n"), PROGRAME_NAME);
    printf(_("Command:\n"));
    printf(_("  start\t: start jacques daemon\n"));
    printf(_("  stop\t: stop jacques daemon and all server processes\n"));
    printf(_
           ("  check\t: check to see if the runtime environment is OK\n"));
    printf("\n");
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


static inline JConfParser *initialize_jacques(void)
{
    if (!j_mkdir_with_parents(RUNTIME_LOCATION, 0755) ||
        !j_mkdir_with_parents(LOG_LOCATION, 0755)) {
        printf("%s\n", strerror(errno));
        exit(-1);
    }

    int n;
    if ((n = jac_check_instance()) > 0) {
        printf(_("jacques is already running!\n"));
        exit(-1);
    } else if (n < 0) {
        printf(_("unable to open %s: %s\n"), PID_FILEPATH,
               strerror(errno));
        exit(-1);
    }

    JConfParser *parser = jac_config_parser();
    char *error = NULL;
    if (!j_conf_parser_parse(parser, CONFIG_FILEPATH, &error)) {
        printf(_("error occurs while parsing configuration - %s\n"),
               error);
        printf(_("exit...\n"));
        j_free(error);
        exit(-1);
    }
    if (!jac_server_check_conf(parser)) {
        printf(_("exit...\n"));
        exit(-1);
    }
    if (!jac_load_modules(parser)) {
        printf(_("exit...\n"));
        exit(-1);
    }
    return parser;
}

static inline void start_jacques(void)
{
    JConfParser *parser = initialize_jacques();
    int pid = jac_daemonize();
    if (!pid || !jac_save_pid(pid)) {
        exit(-1);
    }
    while (1) {
    }
    exit(0);
}

static inline void stop_jacques(void)
{
    printf("stopped!\n");
    exit(0);
}

static inline void check_jacques(void)
{
    JConfParser *parser = jac_config_parser();
    char *error = NULL;
    printf(_("Checking configuration file:\n"));
    if (!j_conf_parser_parse(parser, CONFIG_FILEPATH, &error)) {
        printf(_("\033[31m*ERROR*: %s - %s\033[0m\n"),
               CONFIG_FILEPATH, strerror(errno));
        j_free(error);
    } else {
        printf(_("configuration file is \033[32mOK\033[0m!\n\n"));
        printf(_("Checking %s:\n"), JAC_VIRTUAL_SERVER_SCOPE);
        if (jac_server_check_conf(parser)) {
            printf(_("%s is \033[32mOK\033[0m\n"),
                   JAC_VIRTUAL_SERVER_SCOPE);
        }
    }
    printf("\n");
    printf(_("Checking necessary directory:\n"));
    if (access(CONFIG_LOCATION, F_OK | R_OK)) {
        printf(_("\033[31m*ERROR*: %s - %s\033[0m\n"),
               CONFIG_LOCATION, strerror(errno));
    } else {
        printf(_("configuration directory %s is \033[32mOK\033[0m\n"),
               CONFIG_LOCATION);
    }
    if (access(RUNTIME_LOCATION, F_OK | R_OK | W_OK)) {
        printf(_("\033[31m*ERROR*: %s - %s\033[0m\n"),
               RUNTIME_LOCATION, strerror(errno));
    } else {
        printf(_("runtime directory %s is \033[32mOK\033[0m\n"),
               RUNTIME_LOCATION);
    }
    if (access(LOG_LOCATION, F_OK | R_OK | W_OK)) {
        printf(_("\033[31m*ERROR*: %s - %s\033[0m\n"),
               LOG_LOCATION, strerror(errno));
    } else {
        printf(_("log directory %s is \033[32mOK\033[0m\n"), LOG_LOCATION);
    }
    printf("\n");
    exit(0);
}
