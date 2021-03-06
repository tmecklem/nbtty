/*
    nbtty - A subset of dtach that makes the terminal nonblocking
    Copyright (C) 2004-2016 Ned T. Crigler

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "nbtty.h"

#include <err.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Make sure the binary has a copyright. */
const char copyright[] = "nbtty - version 0.3.0 (C)Copyright 2004-2016 Ned T. Crigler, 2017 Frank Hunleth";

/*
** The original terminal settings. Shared between the master and attach
** processes. The master uses it to initialize the pty, and the attacher uses
** it to restore the original settings.
*/
struct termios orig_term;

int main(int argc, char **argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "No command was specified.");

    /* Save the original terminal settings. */
    if (tcgetattr(0, &orig_term) < 0)
        errx(EXIT_FAILURE, "Attaching to a session requires a terminal.");

    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0)
        err(EXIT_FAILURE, "socketpair");

    if (master_main(&argv[1], sv[0]) != 0)
        return 1;


    close(sv[0]);

    return attach_main(sv[1]);
}
