#!/usr/bin/env python
#
# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).
#
# script/install-gsl-pkg.py
#

from  __future__  import print_function

import argparse
import os

# gsl-lite version, updated by script/update-version.py:

gsl_lite_version = "0.34.0"

# Config:

defCompiler    = 'g++'
defGenerator   = 'Unix Makefiles'
defBuildFolder = './cmake-pkg-install'

# End config.

def cmake( opt, arguments ):
    """Perform CMake command."""
    commandline = "cmake {}".format( arguments )
    if not opt.quiet:
        print( "{}".format( commandline ) )
    os.system( commandline )

def installGslLitePackage( opt ):
    """Install gsl-lite package."""

    if not opt.quiet:
        print( "Installing gsl-lite package:\n")

    # configure:
    build_folder     = opt.build_folder if opt.build_folder else '.'

    opt_build_folder = '-B"{folder}"'                          .format(      folder=opt.build_folder ) if opt.build_folder else ''
    opt_generator    = '-G"{generator}"'                       .format(   generator=opt.generator    ) if opt.generator    else ''
    opt_build_type   = '-DCMAKE_BUILD_TYPE="{config}"'         .format(      config=opt.config       ) if opt.config       else ''
    opt_compiler     = '-DCMAKE_CXX_COMPILER="{compiler}"'     .format(    compiler=opt.compiler     ) if opt.compiler     else ''
    opt_install_pfx  = '-DCMAKE_INSTALL_PREFIX="{install_pfx}"'.format( install_pfx=opt.install_pfx  ) if opt.install_pfx  else ''

    cmake( opt, '-H. {opt_build_folder} {opt_generator} {opt_build_type} {opt_compiler} {opt_install_pfx}'.format(
        opt_build_folder=opt_build_folder
        , opt_generator=opt_generator
        , opt_build_type=opt_build_type
        , opt_compiler=opt_compiler
        , opt_install_pfx=opt_install_pfx ) )

    # install:
    opt_build  = '--build {folder} '  .format( folder=build_folder )
    opt_config = '--config "{config}"'.format( config=opt.config ) if opt.config else ''

    cmake( opt, '{opt_build} {opt_config} --target install'.format(
        opt_build=opt_build, opt_config=opt_config) )

def main():
    """Collect command line options and install gsl-lite package."""

    parser = argparse.ArgumentParser(
        description='Install gsl-lite package.',
        epilog="""""",
        formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='report additional information')

    parser.add_argument(
        '-q', '--quiet',
        action='store_true',
        help='do not report the cmake commands issued')

    parser.add_argument(
        '--build-folder',
        dest='build_folder',
        default=defBuildFolder,
        type=str,
        metavar='F',
        help='the CMake build folder [{deflt}]'.format( deflt=defBuildFolder ) )

    parser.add_argument(
        '--generator',
        dest='generator',
        default=defGenerator,
        type=str,
        metavar='G',
        help='the CMake generator [{deflt}]'.format( deflt=defGenerator ) )

    parser.add_argument(
        '--compiler',
        dest='compiler',
        default=defCompiler,
        type=str,
        metavar='C',
        help='the CMake compiler [{deflt}]'.format( deflt=defCompiler ) )

    parser.add_argument(
        '--config',
        dest='config',
        metavar='C',
        help='the CMake configuration (Debug, Release) []')

    parser.add_argument(
        '--install-prefix',
        dest='install_pfx',
        type=str,
        metavar='P',
        help='the CMake install prefix []')

    opt = parser.parse_args()

    installGslLitePackage( opt )

if __name__ == '__main__':
    main()

# end of file
