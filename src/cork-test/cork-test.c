/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2012, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the COPYING file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libcork/cli.h"
#include "libcork/core.h"
#include "libcork/ds.h"
#include "libcork/os.h"


#define streq(s1, s2)  (strcmp((s1), (s2)) == 0)

#define ri_check_exit(call) \
    do { \
        if ((call) != 0) { \
            fprintf(stderr, "%s\n", cork_error_message()); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define rp_check_exit(call) \
    do { \
        if ((call) == NULL) { \
            fprintf(stderr, "%s\n", cork_error_message()); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)


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
        struct cork_stream_consumer  *consumer;
        printf("And you want the file to be %s.  Sure thing.\n", file_option);

        /* Print the contents of the file to stdout. */
        rp_check_exit(consumer = cork_file_consumer_new(stdout));
        ri_check_exit(cork_consume_file_from_path
                      (consumer, file_option, O_RDONLY));
        cork_stream_consumer_free(consumer);
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


/*-----------------------------------------------------------------------
 * Forking subprocesses
 */

static void
sub_run(int argc, char **argv)
{
    struct cork_subprocess_group  *group;
    struct cork_subprocess  *sub;
    rp_check_exit(group = cork_subprocess_group_new());
    rp_check_exit(sub = cork_subprocess_new_exec(argv[0], argv, NULL, NULL));
    cork_subprocess_group_add(group, sub);
    ri_check_exit(cork_subprocess_group_start(group));
    ri_check_exit(cork_subprocess_group_wait(group));
    cork_subprocess_group_free(group);
}

static struct cork_command  sub =
    cork_leaf_command("sub", "Run a subcommand", "<program> [<options>]",
                      "Runs a subcommand.\n",
                      NULL, sub_run);


/*-----------------------------------------------------------------------
 * Root command
 */

/* [root] cork-test */

static struct cork_command  *root_subcommands[] = {
    &c1, &c2, &sub, NULL
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
