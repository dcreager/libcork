# -*- coding: utf-8 -*-

# Calculates the current version number.  If possible, this is the
# output of “git describe”, modified to conform to the versioning
# scheme that setuptools uses.  If “git describe” returns an error
# (most likely because we're in an unpacked copy of a release tarball,
# rather than in a git working copy), then we fall back on reading the
# contents of the RELEASE-VERSION file.
#
# This will automatically update the RELEASE-VERSION file, if
# necessary.  Note that the RELEASE-VERSION file should *not* be
# checked into git; please add it to your top-level .gitignore file.

__all__ = (
    "ABBREV",
    "get_git_version",
)

from subprocess import Popen, PIPE


ABBREV = 4


def call_git_describe():
    try:
        p = Popen(['git', 'describe', '--abbrev=%d' % ABBREV],
                  stdout=PIPE, stderr=PIPE)
        p.stderr.close()
        line = p.stdout.readlines()[0]
        return line.strip()

    except:
        return None


def read_release_version(filename):
    try:
        f = open(filename, "r")

        try:
            version = f.readlines()[0]
            return version.strip()

        finally:
            f.close()

    except:
        return None


def write_release_version(filename, version):
    f = open(filename, "w")
    try:
        f.write("%s\n" % version)
    finally:
        f.close()


def get_git_version(filename):
    # First try to get the current version using “git describe”.

    version = call_git_describe()

    # If we got something from git-describe, write the version to the
    # output file.

    if version is not None:
        write_release_version(filename, version)

    # Otherwise, fall back on the value that's in RELEASE-VERSION.

    else:
        version = read_release_version(filename)

        # If we still don't have anything, that's an error.

        if version is None:
            raise ValueError("Cannot find the version number!")

    return version
