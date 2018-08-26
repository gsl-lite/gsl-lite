#!/usr/bin/env python
#
# Copyright 2015-2018 by Martin Moene
#
# gsl-lite is based on GSL: Guidelines Support Library,
# https://github.com/microsoft/gsl
#
# This code is licensed under the MIT License (MIT).
#
# example/cmake-pkg/use-gsl-pkg.py
#

from  __future__  import print_function

import argparse
import os

# gsl-lite version, updated by script/update-version.py:

gsl_lite_version = "0.28.0"

# Config:

defCompiler      = 'g++'
defGenerator     = 'Unix Makefiles'
defBuildFolder = './cmake-pkg-build'
defInstallPrefix = '_stage'

# End config.

class cd:
    """Context manager for changing the current working directory"""
    def __init__( self, new_path ):
        self.new_path = os.path.expanduser( new_path )

    def __enter__( self ):
        self.saved_path = os.getcwd()
        os.chdir( self.new_path )

    def __exit__( self, etype, value, traceback ):
        os.chdir( self.saved_path )

def cmake( opt, arguments ):
    """Perform CMake command."""
    commandline = "cmake {}".format( arguments )
    if not opt.quiet:
        print( "{}".format( commandline ) )
    os.system( commandline )

def ctest( opt, arguments ):
    """Perform CTest command."""
    commandline = "ctest {}".format( arguments )
    if not opt.quiet:
        print( "{}".format( commandline ) )
    os.system( commandline )

def  useGslLitePackage( opt ):
    """Use gsl-lite package."""

    if not opt.quiet:
        print( "Using gsl-lite package:\n")

    # configure:
    build_folder     = opt.build_folder if opt.build_folder else '.'

    opt_build_folder = '-B"{folder}"'                          .format(       folder=build_folder )
    opt_generator    = '-G"{generator}"'                       .format(    generator=opt.generator    ) if opt.generator    else ''
    opt_build_type   = '-DCMAKE_BUILD_TYPE="{config}"'         .format(       config=opt.config       ) if opt.config       else ''
    opt_compiler     = '-DCMAKE_CXX_COMPILER="{compiler}"'     .format(     compiler=opt.compiler     ) if opt.compiler     else ''
    opt_install_pfx  = '-DCMAKE_INSTALL_PREFIX="{install_pfx}"'.format(  install_pfx=opt.install_pfx  ) if opt.install_pfx  else ''
    opt_cmake_pfx    = '-DCMAKE_PREFIX_PATH="{cmake_pfx}"'     .format(    cmake_pfx=opt.cmake_pfx    ) if opt.cmake_pfx    else ''
    opt_gsl_lite_dir = '-Dgsl-lite_DIR="{gsl_lite_dir}"'       .format( gsl_lite_dir=opt.gsl_lite_dir ) if opt.gsl_lite_dir else ''

    cmake( opt, '-H. {opt_build_folder} {opt_generator} {opt_build_type} {opt_compiler} {opt_install_pfx} {opt_cmake_pfx} {opt_gsl_lite_dir}'.format(
        opt_build_folder=opt_build_folder
        , opt_generator=opt_generator
        , opt_build_type=opt_build_type
        , opt_compiler=opt_compiler
        , opt_install_pfx=opt_install_pfx
        , opt_cmake_pfx=opt_cmake_pfx
        , opt_gsl_lite_dir=opt_gsl_lite_dir ) )

    # build & test:
    opt_build  = '--build {folder} '  .format( folder=build_folder )
    opt_config = '--config "{config}"'.format( config=opt.config ) if opt.config else ''

    cmake( opt, '{opt_build} {opt_config}'.format( opt_build=opt_build, opt_config=opt_config) )

    with cd( build_folder ):
        ctest( opt, '{opt_config}'.format( opt_config=opt_config) )

def main():
    """Collect command line options and use gsl-lite package."""

    parser = argparse.ArgumentParser(
        description='Use gsl-lite package.',
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
        default=defInstallPrefix,
        type=str,
        metavar='P',
        help='the CMake install prefix [{deflt}]'.format( deflt=defInstallPrefix ) )

    parser.add_argument(
        '--cmake-prefix',
        dest='cmake_pfx',
        type=str,
        metavar='P',
        help='the CMake prefix path []')

    parser.add_argument(
        '--gsl-lite-dir',
        dest='gsl_lite_dir',
        type=str,
        metavar='D',
        help='the gsl-lite directory []')

    opt = parser.parse_args()

    useGslLitePackage( opt )

if __name__ == '__main__':
    main()

# end of file
