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
#include <string.h>

#include "libcork/cli.h"
#include "libcork/core.h"


#define streq(s1, s2)  (strcmp((s1), (s2)) == 0)


/*-----------------------------------------------------------------------
 * Command list
 */

static bool  test_option = false;
static const char  *file_option = NULL;

/* cork-test c1 s1 */

static int
c1_s1_options(int argc, char **argv);

static void
c1_s1_run(int argc, char **argv);

static struct cork_command  c1_s1 =
    cork_leaf_command("s1", "Subcommand 1", "[<options>] <filename>",
                      "This is a pretty cool command.\n",
                      c1_s1_options, c1_s1_run);

static int
c1_s1_options(int argc, char **argv)
{
    if (argc >= 2 && (streq(argv[1], "-t") || streq(argv[1], "--test"))) {
        test_option = true;
        return 2;
    } else {
        return 1;
    }
}

static void
c1_s1_run(int argc, char **argv)
{
    printf("You chose command \"c1 s1\".  Good for you!\n");
    if (test_option) {
        printf("And you gave the --test option!  Look at that.\n");
    }
    if (file_option != NULL) {
        printf("And you want the file to be %s.  Sure thing.\n", file_option);
    }
    exit(EXIT_SUCCESS);
}


/* cork-test c1 s2 */

static void
c1_s2_run(int argc, char **argv)
{
    printf("You chose command \"c1 s2\".  Fantastico!\n");
    if (file_option != NULL) {
        printf("And you want the file to be %s.  Sure thing.\n", file_option);
    }
    exit(EXIT_SUCCESS);
}

static struct cork_command  c1_s2 =
    cork_leaf_command("s2", "Subcommand 2", "[<options>] <filename>",
                      "This is an excellent command.\n",
                      NULL, c1_s2_run);


/* cork-test c1 */

static int
c1_options(int argc, char **argv);

static struct cork_command  *c1_subcommands[] = {
    &c1_s1, &c1_s2, NULL
};

static struct cork_command  c1 =
    cork_command_set("c1", "Command 1 (now with subcommands)",
                     c1_options, c1_subcommands);

static int
c1_options(int argc, char **argv)
{
    if (argc >= 3) {
        if (streq(argv[1], "-f") || streq(argv[1], "--file")) {
            file_option = argv[2];
            return 3;
        }
    }

    if (argc >= 2) {
        if (memcmp(argv[1], "--file=", 7) == 0) {
            file_option = argv[1] + 7;
            return 2;
        }
    }

    return 1;
}


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
