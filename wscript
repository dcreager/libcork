# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2010, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

from version import *

#--------------------------
# Edit the variables below

APPNAME = "PLEASE_DEFINE_APPNAME"
VERSION = get_git_version("RELEASE-VERSION")

SUBDIRS = [
    "include",
    "src",
]

# The tests subdirectory is only included if the user runs "waf test"
# or "waf build_tests".

# Everything below this is boilerplate
#--------------------------------------


def set_options(opt):
    map(opt.recurse, SUBDIRS)

    opt.add_option(
        "--build-kind",
        action="store",
        default="debug,release",
        help="which variants to build"
    )


def configure(conf):
    conf.env.DEBUG_CFLAGS = [
        "-Wall",
        "-Werror",
        "-O2",
        "-g",
    ]

    conf.env.RELEASE_CFLAGS = [
        "-Wall",
        "-Werror",
        "-O3",
    ]

    conf.env.DEBUG_DEFINES = []
    conf.env.RELEASE_DEFINES = []

    conf.env.APPNAME = APPNAME
    conf.env.VERSION = VERSION

    map(conf.recurse, SUBDIRS)

    # Create debug and release builds

    dbg = conf.env.copy()
    rel = conf.env.copy()

    dbg.set_variant("debug")
    conf.set_env_name("debug", dbg)
    conf.setenv("debug")
    conf.env.CCFLAGS = conf.env.DEBUG_CFLAGS
    conf.env.CCDEFINES = conf.env.DEBUG_DEFINES
    conf.env.CXXFLAGS = conf.env.DEBUG_CFLAGS
    conf.env.CXXDEFINES = conf.env.DEBUG_DEFINES

    rel.set_variant("release")
    conf.set_env_name("release", rel)
    conf.setenv("release")
    conf.env.CCFLAGS = conf.env.RELEASE_CFLAGS
    conf.env.CCDEFINES = conf.env.RELEASE_DEFINES
    conf.env.CXXFLAGS = conf.env.RELEASE_CFLAGS
    conf.env.CXXDEFINES = conf.env.RELEASE_DEFINES


def build(bld):
    map(bld.recurse, SUBDIRS)

    for obj in bld.all_task_gen[:]:
        debug_obj = obj.clone("debug")
        release_obj = obj.clone("release")

        obj.posted = True
        if hasattr(obj, "unit_test"):
            obj.unit_test = False

        # Disable each variant if it's not included in the
        # --build-kind command-line option.

        import Options
        kinds = Options.options.build_kind.split(",")

        if "debug" not in kinds:
            debug_obj.posted = True
        if "release" not in kinds:
            release_obj.posted = True


def test(tst):
    import Scripting
    Scripting.commands.insert(0, "run_tests")
    Scripting.commands.insert(0, "build")
    Scripting.commands.insert(0, "add_test_dirs")


def add_test_dirs(tst):
    SUBDIRS.append("tests")


def run_tests(tst):
    import unittestw
    ut = unittestw.unit_test()
    ut.run_if_waf_does = "test"
    ut.run()
    ut.print_results()


def build_tests(tst):
    import Scripting
    Scripting.commands.insert(0, "build")
    Scripting.commands.insert(0, "add_test_dirs")
