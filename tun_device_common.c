/*
 * Tun device configuration, post up/down program execution
 *
 * Copyright (C) 2009 Florent Bondoux
 *
 * This file is part of Campagnol.
 *
 * Campagnol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Campagnol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Campagnol.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * 
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 *
 */

#include "tun_device.h"

/* Replace the special variables %D %V ... in s and write the result in sb
 */
static void replace_args(strlib_buf_t *sb, const char *s, const char *device) {
    const char *src;

    strlib_reset(sb);
    src = s;

    while (*src) {
        switch (*src) {
            case '%':
                switch (*(src + 1)) {
                    case '%':
                        strlib_push(sb, '%');
                        break;
                    case 'D': // device
                        strlib_appendf(sb, "'%s'", device);
                        break;
                    case 'V': // VPN IP
                        strlib_appendf(sb, "%s", inet_ntoa(config.vpnIP));
                        break;
                    case 'M': // MTU
                        strlib_appendf(sb, "%d", config.tun_mtu);
                        break;
                    case 'N': // netmask as a string
                        strlib_appendf(sb, "'%s'", config.network);
                        break;
                    default:
                        strlib_push(sb, '%');
                        strlib_push(sb, *(src + 1));
                        break;
                }
                src += 2;
                break;
            default:
                strlib_push(sb, *src);
                src++;
                break;
        }
    }
}

/* execute the commands in programs
 */
static void exec_internal(const char * const * programs, const char *device) {
    int r;
    strlib_buf_t sb;
    strlib_init(&sb);
    if (programs != NULL) {
        while (*programs) {
            replace_args(&sb, *programs, device);
            printf("Running: %s \n", sb.s);
            r = system(sb.s);
            printf("Exited with status %d \n", WEXITSTATUS(r));
            programs++;
        }
    }
    strlib_free(&sb);
}

void exec_up(const char *device) {
    exec_internal((const char * const *) tun_default_up, device);
}

void exec_down(const char *device) {
    exec_internal((const char * const *) tun_default_down, device);
}
