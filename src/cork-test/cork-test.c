/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "libcork/cli.h"


/*-----------------------------------------------------------------------
 * Command list
 */

/* cork-test c1 s1 */

static void
c1_s1_run(int argc, char **argv)
{
    printf("You chose command \"c1 s1\".  Good for you!\n");
    exit(EXIT_SUCCESS);
}

static struct cork_command  c1_s1 =
    cork_leaf_command("s1", "Subcommand 1", "[<options>] <filename>",
                      "This is a pretty cool command.\n",
                      NULL, c1_s1_run);


/* cork-test c1 s2 */

static void
c1_s2_run(int argc, char **argv)
{
    printf("You chose command \"c1 s2\".  Fantastico!\n");
    exit(EXIT_SUCCESS);
}

static struct cork_command  c1_s2 =
    cork_leaf_command("s2", "Subcommand 2", "[<options>] <filename>",
                      "This is an excellent command.\n",
                      NULL, c1_s2_run);


/* cork-test c1 */

static struct cork_command  *c1_subcommands[] = {
    &c1_s1, &c1_s2, NULL
};

static struct cork_command  c1 =
    cork_command_set("c1", "Command 1 (now with subcommands)", NULL,
                     c1_subcommands);


/* cork-test c2 */

static void
c2_run(int argc, char **argv)
{
    printf("You chose command \"c2\".  That's pretty good.\n");
    exit(EXIT_SUCCESS);
}

static struct cork_command  c2 =
    cork_leaf_command("c2", "Command 2", "[<options>] <filename>",
                      "This command is pretty decent.\n",
                      NULL, c2_run);


/* [root] cork-test */

static struct cork_command  *root_subcommands[] = {
    &c1, &c2, NULL
};

static struct cork_command  root_command =
    cork_command_set("cork-test", NULL, NULL, root_subcommands);


/*-----------------------------------------------------------------------
 * Entry point
 */

int
main(int argc, char **argv)
{
    return cork_command_main(&root_command, argc, argv);
}
