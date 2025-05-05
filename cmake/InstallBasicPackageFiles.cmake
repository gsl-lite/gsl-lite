# SPDX-FileCopyrightText: 2012-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#[=======================================================================[.rst:
InstallBasicPackageFiles
------------------------

A helper module to make your package easier to be found by other
projects.


.. command:: install_basic_package_files

Create and install a basic version of cmake config files for your
project::

 install_basic_package_files(<Name>
                             COMPATIBILITY <compatibility>
                             [VERSION <version>]
                             [ARCH_INDEPENDENT]
                             [NO_EXPORT | EXPORT <export>] # (default = "EXPORT <Name>")
                             [NO_SET_AND_CHECK_MACRO]
                             [NO_CHECK_REQUIRED_COMPONENTS_MACRO]
                             [VARS_PREFIX <prefix>] # (default = "<Name>")
                             [EXPORT_DESTINATION <destination>]
                             [INSTALL_DESTINATION <destination>]
                             [NAMESPACE <namespace>] # (default = "<Name>::")
                             [EXTRA_PATH_VARS_SUFFIX path1 [path2 ...]]
                             [CONFIG_TEMPLATE <file>]
                             [UPPERCASE_FILENAMES | LOWERCASE_FILENAMES]
                             [DEPENDENCIES <dependency1> "<dependency2> [...]" ...]
                             [PRIVATE_DEPENDENCIES <dependency1> "<dependency2> [...]" ...]
                             [INCLUDE_FILE <file> | INCLUDE_CONTENT <content>]
                             [COMPONENT <component>] # (default = "<Name>")
                            )

Depending on ``UPPERCASE_FILENAMES`` and ``LOWERCASE_FILENAMES``, this
function generates 3 files:

 - ``<Name>ConfigVersion.cmake`` or ``<name>-config-version.cmake``
 - ``<Name>Config.cmake`` or ``<name>-config.cmake``
 - ``<Name>Targets.cmake`` or ``<name>-targets.cmake``

If neither ``UPPERCASE_FILENAMES`` nor ``LOWERCASE_FILENAMES`` is
set, a file ``<Name>Config.cmake.in`` or
``<name>-config.cmake.in`` is searched, and the convention
is chosed according to the file found. If no file was found, the
uppercase convention is used.

The ``DEPENDENCIES`` argument can be used to set a list of dependencies
that will be searched using the :cmake:command:`find_dependency` command
from the :module:`CMakeFindDependencyMacro` module.
Dependencies can be followed by any of the possible
:cmake:command:`find_dependency` argument.
In this case, all the arguments must be specified within double quotes (e.g.
``"<dependency> 1.0.0 EXACT"``, or ``"<dependency> CONFIG"``).
The ``PRIVATE_DEPENDENCIES`` argument is similar to ``DEPENDENCIES``, but
these dependencies are included only when :variable:`BUILD_SHARED_LIBS` is
``OFF``.
If a libraries is declared ``STATIC``, ``OBJECT`` or ``INTERFACE``, and they
link to some dependency, these should be added using the ``DEPENDENCIES``
argument, since the ``PRIVATE_DEPENDENCIES`` argument would work only when
:variable:`BUILD_SHARED_LIBS` is disabled.

When using a custom template file, the ``@PACKAGE_DEPENDENCIES@``
string is replaced with the code checking for the dependencies
specified by these two argument.

If the ``ARCH_INDEPENDENT`` option is enabled, the installed package version
will be considered compatible even if it was built for a different
architecture than the requested architecture.

Each file is generated twice, one for the build directory and one for
the installation directory.  The ``INSTALL_DESTINATION`` argument can be
passed to install the files in a location different from the default
one (``${CMAKE_INSTALL_DATADIR}/cmake/${Name}`` if the ``ARCH_INDEPENDENT``
option is enabled, ``${CMAKE_INSTALL_LIBDIR}/cmake/${Name}`` otherwise).
The ``EXPORT_DESTINATION`` argument can be passed to
generate the files in the build tree in a location different from the default
one (``CMAKE_BINARY_DIR``).  If this is a relative path, it is
considered relative to the ``CMAKE_CURRENT_BINARY_DIR`` directory.

The build directory is exported to the CMake user package registry if the
build option ``CMAKE_EXPORT_PACKAGE_REGISTRY`` is set.

The ``<Name>ConfigVersion.cmake`` file is generated using
:cmake:command:`write_basic_package_version_file`. The ``VERSION``,
``COMPATIBILITY``, and ``ARCH_INDEPENDENT``arguments are passed to this
function.

``VERSION`` shall be in the form ``<major>[.<minor>[.<patch>[.<tweak>]]]]``.
If no ``VERSION`` is given, the ``PROJECT_VERSION`` variable is used.
If this hasn’t been set, it errors out.  The ``VERSION`` argument is also used
to replace the ``@PACKAGE_VERSION@`` string in the configuration file.

``COMPATIBILITY`` shall be any of the options accepted by the
:cmake:command:`write_basic_package_version_file` command
(``AnyNewerVersion``, ``SameMajorVersion``, ``SameMinorVersion`` [CMake 3.11],
or ``ExactVersion``).
These options are explained in :cmake:command:`write_basic_package_version_file`
command documentation.
If your project has more elaborate version matching rules, you will need to
write your own custom ConfigVersion.cmake file instead of using this macro.

The ``<Name>Config.cmake`` file is generated using
:cmake:command:`configure_package_config_file`. The
``NO_SET_AND_CHECK_MACRO``, ``NO_CHECK_REQUIRED_COMPONENTS_MACRO``, and
arguments are passed to this function.

By default :command:`install_basic_package_files` also generates the two helper
macros ``set_and_check()`` and ``check_required_components()`` into the
``<Name>Config.cmake`` file. ``set_and_check()`` should be used instead of the
normal :cmake:command:`set()` command for setting directories and file locations.
Additionally to setting the variable it also checks that the referenced file
or directory actually exists and fails with a ``FATAL_ERROR`` otherwise.
This makes sure that the created ``<Name>Config.cmake`` file does not contain
wrong references.
When using the ``NO_SET_AND_CHECK_MACRO, this macro is not generated into the
``<Name>Config.cmake`` file.

By default, :command:`install_basic_package_files` append a call to
``check_required_components(<Name>)`` in ``<Name>Config.cmake`` file if the
package supports components. This macro checks whether all requested,
non-optional components have been found, and if this is not the case, sets the
``<Name>_FOUND`` variable to ``FALSE``, so that the package is considered to
be not found. It does that by testing the ``<Name>_<Component>_FOUND``
variables for all requested required components. When using the
``NO_CHECK_REQUIRED_COMPONENTS_MACRO`` option, this macro is not generated
into the ``<Name>Config.cmake`` file.

Finally, the files in the build and install directory are exactly the same.

See the documentation of :module:`CMakePackageConfigHelpers` module for
further information and references therein.

If the ``CONFIG_TEMPLATE`` argument is passed, the specified file
is used as template for generating the configuration file, otherwise
this module expects to find a ``<Name>Config.cmake.in`` or
``<name>-config.cmake.in`` file either in current source directory.
If the file does not exist, a very basic file is created.

A set of variables are checked and passed to
:cmake:command:`configure_package_config_file` as ``PATH_VARS``. For each of the
``SUFFIX`` considered, if one of the variables::

    <VARS_PREFIX>_(BUILD|INSTALL)_<SUFFIX>
    (BUILD|INSTALL)_<VARS_PREFIX>_<SUFFIX>

is defined, the ``<VARS_PREFIX>_<SUFFIX>`` variable will be defined
before configuring the package.  In order to use that variable in the
config file, you have to add a line::

  set_and_check(<VARS_PREFIX>_<SUFFIX> \"@PACKAGE_<VARS_PREFIX>_<SUFFIX>@\")

if the path must exist or just::

  set(<VARS_PREFIX>_<SUFFIX> \"@PACKAGE_<VARS_PREFIX>_<SUFFIX>@\")

if the path could be missing.

These variable will have different values whether you are using the
package from the build tree or from the install directory.  Also these
files will contain only relative paths, meaning that you can move the
whole installation and the CMake files will still work.

Default ``PATH_VARS`` suffixes are::

  BINDIR          BIN_DIR
  SBINDIR         SBIN_DIR
  LIBEXECDIR      LIBEXEC_DIR
  SYSCONFDIR      SYSCONF_DIR
  SHAREDSTATEDIR  SHAREDSTATE_DIR
  LOCALSTATEDIR   LOCALSTATE_DIR
  LIBDIR          LIB_DIR
  INCLUDEDIR      INCLUDE_DIR
  OLDINCLUDEDIR   OLDINCLUDE_DIR
  DATAROOTDIR     DATAROOT_DIR
  DATADIR         DATA_DIR
  INFODIR         INFO_DIR
  LOCALEDIR       LOCALE_DIR
  MANDIR          MAN_DIR
  DOCDIR          DOC_DIR

more suffixes can be added using the ``EXTRA_PATH_VARS_SUFFIX``
argument.


The ``<Name>Targets.cmake`` is generated using :cmake:command:`export(EXPORT)`
in the build tree and :cmake:command:`install(EXPORT)` in the installation
directory. The targets are exported using the value for the ``NAMESPACE``
argument as namespace.
The export can be passed using the ``EXPORT`` argument. If no export is
used (e.g. for a CMake script library), pass ``NO_EXPORT``.

If the ``INCLUDE_FILE`` argument is passed, the content of the specified file
(which might contain ``@variables@``) is appended to the generated
``<Name>Config.cmake`` file.
If the ``INCLUDE_CONTENT`` argument is passed, the specified content
(which might contain ``@variables@``) is appended to the generated
``<Name>Config.cmake`` file.
When a ``CONFIG_TEMPLATE`` is passed, or a ``<Name>ConfigVersion.cmake.in`` or
a ``<name>-config-version.cmake.in file is available, these 2 arguments are
used to replace the ``@INCLUDED_CONTENT@`` string in this file.
This allows one to inject custom code to this file, useful e.g. to set
additional variables which are loaded by downstream projects.

Note that content specified with ``INCLUDE_FILE`` or ``INCLUDE_CONTENT``
cannot reference any of the ``PATH_VARS`` because this content is not
expanded by :cmake:command:`configure_package_config_file`.

If the ``COMPONENT`` argument is passed, it is forwarded to the
:cmake:command:`install` commands, otherwise ``<Name>`` is used.
#]=======================================================================]


if(COMMAND install_basic_package_files)
  return()
endif()


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeParseArguments)


function(INSTALL_BASIC_PACKAGE_FILES _Name)

  set(_options ARCH_INDEPENDENT
               NO_EXPORT
               NO_SET_AND_CHECK_MACRO
               NO_CHECK_REQUIRED_COMPONENTS_MACRO
               UPPERCASE_FILENAMES
               LOWERCASE_FILENAMES
               NO_COMPATIBILITY_VARS # Deprecated
               ENABLE_COMPATIBILITY_VARS) # Deprecated
  set(_oneValueArgs VERSION
                    COMPATIBILITY
                    EXPORT
                    FIRST_TARGET # Deprecated
                    TARGETS_PROPERTY # Deprecated
                    VARS_PREFIX
                    EXPORT_DESTINATION
                    INSTALL_DESTINATION
                    DESTINATION # Deprecated
                    NAMESPACE
                    CONFIG_TEMPLATE
                    INCLUDE_FILE
                    INCLUDE_CONTENT
                    COMPONENT)
  set(_multiValueArgs EXTRA_PATH_VARS_SUFFIX
                      TARGETS # Deprecated
                      TARGETS_PROPERTIES # Deprecated
                      DEPENDENCIES
                      PRIVATE_DEPENDENCIES)
  cmake_parse_arguments(_IBPF "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" "${ARGN}")

  if(NOT DEFINED _IBPF_VARS_PREFIX)
    set(_IBPF_VARS_PREFIX ${_Name})
  endif()

  if(NOT DEFINED _IBPF_VERSION)
    if(NOT DEFINED PROJECT_VERSION)
      message(FATAL_ERROR "VERSION argument is required (PROJECT_VERSION is not defined)")
    endif()
    set(_IBPF_VERSION ${PROJECT_VERSION})
  endif()

  if(NOT DEFINED _IBPF_COMPATIBILITY)
    message(FATAL_ERROR "COMPATIBILITY argument is required")
  endif()

  unset(_arch_independent)
  if(_IBPF_ARCH_INDEPENDENT)
    set(_arch_independent ARCH_INDEPENDENT)
  endif()

  if(_IBPF_UPPERCASE_FILENAMES AND _IBPF_LOWERCASE_FILENAMES)
    message(FATAL_ERROR "UPPERCASE_FILENAMES and LOWERCASE_FILENAMES arguments cannot be used together")
  endif()

  if(DEFINED _IBPF_INCLUDE_FILE AND DEFINED _IBPF_INCLUDE_CONTENT)
    message(FATAL_ERROR "INCLUDE_FILE and INCLUDE_CONTENT arguments cannot be used together")
  endif()

  # Prepare install and export commands
  unset(_targets)
  set(_install_cmd EXPORT ${_Name})
  set(_export_cmd EXPORT ${_Name})

  if(DEFINED _IBPF_EXPORT)
    if(_IBPF_NO_EXPORT OR DEFINED _IBPF_TARGETS OR DEFINED _IBPF_TARGETS_PROPERTIES OR DEFINED _IBPF_TARGETS_PROPERTIES)
      message(FATAL_ERROR "EXPORT cannot be used with NO_EXPORT, TARGETS, TARGETS_PROPERTY, or TARGETS_PROPERTIES")
    endif()

    set(_export_cmd EXPORT ${_IBPF_EXPORT})
    set(_install_cmd EXPORT ${_IBPF_EXPORT})

  elseif(_IBPF_NO_EXPORT)
    if(DEFINED _IBPF_TARGETS OR DEFINED _IBPF_TARGETS_PROPERTIES OR DEFINED _IBPF_TARGETS_PROPERTIES)
      message(FATAL_ERROR "NO_EXPORT cannot be used with TARGETS, TARGETS_PROPERTY, or TARGETS_PROPERTIES")
    endif()

  elseif(DEFINED _IBPF_TARGETS)
    message(DEPRECATION "TARGETS is deprecated. Use EXPORT instead")

    if(DEFINED _IBPF_TARGETS_PROPERTY OR DEFINED _IBPF_TARGETS_PROPERTIES)
      message(FATAL_ERROR "TARGETS cannot be used with TARGETS_PROPERTY or TARGETS_PROPERTIES")
    endif()

    set(_targets ${_IBPF_TARGETS})
    set(_export_cmd TARGETS ${_IBPF_TARGETS})

  elseif(DEFINED _IBPF_TARGETS_PROPERTY)
    message(DEPRECATION "TARGETS_PROPERTY is deprecated. Use EXPORT instead")

    if(DEFINED _IBPF_TARGETS_PROPERTIES)
      message(FATAL_ERROR "TARGETS_PROPERTIES cannot be used with TARGETS_PROPERTIES")
    endif()

    get_property(_targets GLOBAL PROPERTY ${_IBPF_TARGETS_PROPERTY})
    set(_export_cmd TARGETS ${_targets})

  elseif(DEFINED _IBPF_TARGETS_PROPERTIES)
    message(DEPRECATION "TARGETS_PROPERTIES is deprecated. Use EXPORT instead")

    set(_targets "") # Defined but empty
    foreach(_prop ${_IBPF_TARGETS_PROPERTIES})
      get_property(_prop_val GLOBAL PROPERTY ${_prop})
      list(APPEND _targets ${_prop_val})
    endforeach()
    set(_export_cmd TARGETS ${_targets})

  endif()

  # Path for installed cmake files
  if(DEFINED _IBPF_DESTINATION)
    message(DEPRECATION "DESTINATION is deprecated. Use INSTALL_DESTINATION instead")
    if(NOT DEFINED _IBPF_INSTALL_DESTINATION)
      set(_IBPF_INSTALL_DESTINATION ${_IBPF_DESTINATION})
    endif()
  endif()

  # If not set by the user, choose an adequate destination
  if(NOT DEFINED _IBPF_INSTALL_DESTINATION)
    if(_IBPF_ARCH_INDEPENDENT)
      set(_IBPF_INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/${_Name})
    else()
      set(_IBPF_INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${_Name})
    endif()
  endif()

  # FIRST_TARGET is no longer used
  if(DEFINED _IBPF_FIRST_TARGET)
    message(DEPRECATION "FIRST_TARGET is deprecated.")
  endif()

  # NO_COMPATIBILITY_VARS and ENABLE_COMPATIBILITY_VARS cannot be used together
  if(_IBPF_NO_COMPATIBILITY_VARS AND _ENABLE_COMPATIBILITY_VARS)
    message(FATAL_ERROR "NO_COMPATIBILITY_VARS and ENABLE_COMPATIBILITY_VARS cannot be used together")
  endif()
  # NO_COMPATIBILITY_VARS is deprecated
  if(_IBPF_NO_COMPATIBILITY_VARS)
    message(DEPRECATION "NO_COMPATIBILITY_VARS is deprecated.")
  endif()
  # ENABLE_COMPATIBILITY_VARS is deprecated
  if(_IBPF_ENABLE_COMPATIBILITY_VARS)
    message(DEPRECATION "ENABLE_COMPATIBILITY_VARS is deprecated.")
  endif()
  # ENABLE_COMPATIBILITY_VARS does not work with EXPORT
  if(NOT DEFINED _targets AND _IBPF_ENABLE_COMPATIBILITY_VARS)
    message(FATAL_ERROR "ENABLE_COMPATIBILITY_VARS does not work with EXPORT")
  endif()
  # ENABLE_COMPATIBILITY_VARS can be enabled for projects still using targets
  if(DEFINED _targets AND NOT _IBPF_NO_COMPATIBILITY_VARS AND NOT _IBPF_ENABLE_COMPATIBILITY_VARS)
    message(DEPRECATION "Compatibility variables are no longer generated. Use ENABLE_COMPATIBILITY_VARS to re-enable them (deprecated) or define them using either INCLUDE_FILE or INCLUDE_CONTENT (recommended).")
  endif()

  if(NOT DEFINED _IBPF_EXPORT_DESTINATION)
    set(_IBPF_EXPORT_DESTINATION "${CMAKE_BINARY_DIR}")
  elseif(NOT IS_ABSOLUTE "${_IBPF_EXPORT_DESTINATION}")
    set(_IBPF_EXPORT_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${_IBPF_EXPORT_DESTINATION}")
  endif()

  if(NOT DEFINED _IBPF_NAMESPACE)
    set(_IBPF_NAMESPACE "${_Name}::")
  endif()

  if(NOT DEFINED _IBPF_COMPONENT)
    set(_IBPF_COMPONENT "${_Name}")
  endif()

  if(_IBPF_NO_SET_AND_CHECK_MACRO)
    list(APPEND configure_package_config_file_extra_args NO_SET_AND_CHECK_MACRO)
  endif()

  if(_IBPF_NO_CHECK_REQUIRED_COMPONENTS_MACRO)
    list(APPEND configure_package_config_file_extra_args NO_CHECK_REQUIRED_COMPONENTS_MACRO)
  endif()



  # Set input file for config, and ensure that _IBPF_UPPERCASE_FILENAMES
  # and _IBPF_LOWERCASE_FILENAMES are set correctly
  unset(_config_cmake_in)
  set(_generate_file 0)
  if(DEFINED _IBPF_CONFIG_TEMPLATE)
    if(NOT EXISTS "${_IBPF_CONFIG_TEMPLATE}")
      message(FATAL_ERROR "Config template file \"${_IBPF_CONFIG_TEMPLATE}\" not found")
    endif()
    set(_config_cmake_in "${_IBPF_CONFIG_TEMPLATE}")
    if(NOT _IBPF_UPPERCASE_FILENAMES AND NOT _IBPF_LOWERCASE_FILENAMES)
      if("${_IBPF_CONFIG_TEMPLATE}" MATCHES "${_Name}Config.cmake.in")
        set(_IBPF_UPPERCASE_FILENAMES 1)
      elseif("${_IBPF_CONFIG_TEMPLATE}" MATCHES "${_name}-config.cmake.in")
        set(_IBPF_LOWERCASE_FILENAMES 1)
      else()
        set(_IBPF_UPPERCASE_FILENAMES 1)
      endif()
    endif()
  else()
    string(TOLOWER "${_Name}" _name)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_Name}Config.cmake.in")
      set(_config_cmake_in "${CMAKE_CURRENT_SOURCE_DIR}/${_Name}Config.cmake.in")
      if(NOT _IBPF_UPPERCASE_FILENAMES AND NOT _IBPF_LOWERCASE_FILENAMES)
        set(_IBPF_UPPERCASE_FILENAMES 1)
      endif()
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_name}-config.cmake.in")
      set(_config_cmake_in "${CMAKE_CURRENT_SOURCE_DIR}/${_name}-config.cmake.in")
      if(NOT _IBPF_UPPERCASE_FILENAMES AND NOT _IBPF_LOWERCASE_FILENAMES)
        set(_IBPF_LOWERCASE_FILENAMES 1)
      endif()
    else()
      set(_generate_file 1)
      if(_IBPF_LOWERCASE_FILENAMES)
        set(_config_cmake_in "${CMAKE_CURRENT_BINARY_DIR}/${_name}-config.cmake.in")
      else()
        set(_config_cmake_in "${CMAKE_CURRENT_BINARY_DIR}/${_Name}Config.cmake.in")
        set(_IBPF_UPPERCASE_FILENAMES 1)
      endif()
    endif()
  endif()

  # Set input file containing user variables
  if(DEFINED _IBPF_INCLUDE_FILE)
    if(NOT IS_ABSOLUTE "${_IBPF_INCLUDE_FILE}")
      if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_IBPF_INCLUDE_FILE}")
        set(_IBPF_INCLUDE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${_IBPF_INCLUDE_FILE}")
      endif()
    endif()
    if(NOT EXISTS "${_IBPF_INCLUDE_FILE}")
      message(FATAL_ERROR "File \"${_IBPF_INCLUDE_FILE}\" not found")
    endif()
    file(READ ${_IBPF_INCLUDE_FILE} _IBPF_INCLUDE_CONTENT)
  endif()

  if(DEFINED _IBPF_INCLUDE_CONTENT)
    string(CONFIGURE ${_IBPF_INCLUDE_CONTENT}
           _IBPF_INCLUDE_CONTENT
           @ONLY)
    set(INCLUDED_CONTENT
"#### Expanded from INCLUDE_FILE/INCLUDE_CONTENT by install_basic_package_files() ####

${_IBPF_INCLUDE_CONTENT}

#####################################################################################
")
  endif()

  # Backwards compatibility
  if(NOT _generate_file AND DEFINED _IBPF_INCLUDE_FILE)
    file(READ ${_config_cmake_in} _config_cmake_in_content)
    if("${_config_cmake_in_content}" MATCHES "@INCLUDED_FILE_CONTENT@")
      message(DEPRECATION "The @INCLUDED_FILE_CONTENT@ variable is deprecated in favour of @INCLUDED_CONTENT@")
      set(INCLUDED_FILE_CONTENT "${INCLUDED_CONTENT}")
    endif()
  endif()

  # Select output file names
  if(_IBPF_UPPERCASE_FILENAMES)
    set(_config_filename ${_Name}Config.cmake)
    set(_version_filename ${_Name}ConfigVersion.cmake)
    set(_targets_filename ${_Name}Targets.cmake)
  elseif(_IBPF_LOWERCASE_FILENAMES)
    set(_config_filename ${_name}-config.cmake)
    set(_version_filename ${_name}-config-version.cmake)
    set(_targets_filename ${_name}-targets.cmake)
  endif()


  # If the template config file does not exist, write a basic one
  if(_generate_file)
    # Generate the compatibility code
    unset(_compatibility_vars)
    if(_IBPF_ENABLE_COMPATIBILITY_VARS)
      unset(_get_include_dir_code)
      unset(_set_include_dir_code)
      unset(_target_list)
      foreach(_target ${_targets})
        list(APPEND _target_list ${_IBPF_NAMESPACE}${_target})
      endforeach()
      if(DEFINED ${_IBPF_VARS_PREFIX}_BUILD_INCLUDEDIR OR
         DEFINED BUILD_${_IBPF_VARS_PREFIX}_INCLUDEDIR OR
         DEFINED ${_IBPF_VARS_PREFIX}_INSTALL_INCLUDEDIR OR
         DEFINED INSTALL_${_IBPF_VARS_PREFIX}_INCLUDEDIR)
        list(APPEND _include_dir_list "\"\@PACKAGE_${_IBPF_VARS_PREFIX}_INCLUDEDIR\@\"")
      elseif(DEFINED ${_IBPF_VARS_PREFIX}_BUILD_INCLUDE_DIR OR
             DEFINED BUILD_${_IBPF_VARS_PREFIX}_INCLUDE_DIR OR
             DEFINED ${_IBPF_VARS_PREFIX}_INSTALL_INCLUDE_DIR OR
             DEFINED INSTALL_${_IBPF_VARS_PREFIX}_INCLUDE_DIR)
        list(APPEND _include_dir_list "\"\@PACKAGE_${_IBPF_VARS_PREFIX}_INCLUDE_DIR\@\"")
      else()
        unset(_include_dir_list)
        foreach(_target ${_targets})
          list(APPEND _include_dir_list "\$<TARGET_PROPERTY:${_IBPF_NAMESPACE}${_target},INTERFACE_INCLUDE_DIRECTORIES>")
        endforeach()
        string(REPLACE ";" " " _include_dir_list "${_include_dir_list}")
        string(REPLACE ";" " " _target_list "${_target_list}")
        set(_set_include_dir "")
      endif()
      set(_compatibility_vars
"# Compatibility\nset(${_Name}_LIBRARIES ${_target_list})
set(${_Name}_INCLUDE_DIRS ${_include_dir_list})
if(NOT \"\${${_Name}_INCLUDE_DIRS}\" STREQUAL \"\")
  list(REMOVE_DUPLICATES ${_Name}_INCLUDE_DIRS)
endif()
")
    endif()

    if(_IBPF_NO_EXPORT)
      set(_include_targets_cmd "")
    else()
      set(_include_targets_cmd "include(\"\${CMAKE_CURRENT_LIST_DIR}/${_targets_filename}\")")
    endif()

    # Write the file
    file(WRITE "${_config_cmake_in}"
"set(${_IBPF_VARS_PREFIX}_VERSION \@PACKAGE_VERSION\@)

\@PACKAGE_INIT\@

\@PACKAGE_DEPENDENCIES\@

${_include_targets_cmd}

${_compatibility_vars}

\@INCLUDED_CONTENT\@
")
  endif()

  # Make relative paths absolute (needed later on) and append the
  # defined variables to _(build|install)_path_vars_suffix
  foreach(p BINDIR          BIN_DIR
            SBINDIR         SBIN_DIR
            LIBEXECDIR      LIBEXEC_DIR
            SYSCONFDIR      SYSCONF_DIR
            SHAREDSTATEDIR  SHAREDSTATE_DIR
            LOCALSTATEDIR   LOCALSTATE_DIR
            LIBDIR          LIB_DIR
            INCLUDEDIR      INCLUDE_DIR
            OLDINCLUDEDIR   OLDINCLUDE_DIR
            DATAROOTDIR     DATAROOT_DIR
            DATADIR         DATA_DIR
            INFODIR         INFO_DIR
            LOCALEDIR       LOCALE_DIR
            MANDIR          MAN_DIR
            DOCDIR          DOC_DIR
            ${_IBPF_EXTRA_PATH_VARS_SUFFIX})
    if(DEFINED ${_IBPF_VARS_PREFIX}_BUILD_${p})
      list(APPEND _build_path_vars_suffix ${p})
      list(APPEND _build_path_vars "${_IBPF_VARS_PREFIX}_${p}")
    endif()
    if(DEFINED BUILD_${_IBPF_VARS_PREFIX}_${p})
      list(APPEND _build_path_vars_suffix ${p})
      list(APPEND _build_path_vars "${_IBPF_VARS_PREFIX}_${p}")
    endif()
    if(DEFINED ${_IBPF_VARS_PREFIX}_INSTALL_${p})
      list(APPEND _install_path_vars_suffix ${p})
      list(APPEND _install_path_vars "${_IBPF_VARS_PREFIX}_${p}")
    endif()
    if(DEFINED INSTALL_${_IBPF_VARS_PREFIX}_${p})
      list(APPEND _install_path_vars_suffix ${p})
      list(APPEND _install_path_vars "${_IBPF_VARS_PREFIX}_${p}")
    endif()
  endforeach()


  # <Name>ConfigVersion.cmake file (same for build tree and intall)
  write_basic_package_version_file("${_IBPF_EXPORT_DESTINATION}/${_version_filename}"
                                   VERSION ${_IBPF_VERSION}
                                   COMPATIBILITY ${_IBPF_COMPATIBILITY}
                                   ${_arch_independent})
  install(FILES "${_IBPF_EXPORT_DESTINATION}/${_version_filename}"
          DESTINATION ${_IBPF_INSTALL_DESTINATION}
          COMPONENT ${_IBPF_COMPONENT})


  # Prepare PACKAGE_DEPENDENCIES variable
  set(_need_private_deps 0)
  if(NOT BUILD_SHARED_LIBS)
    set(_need_private_deps 1)
  endif()

  unset(PACKAGE_DEPENDENCIES)
  if(DEFINED _IBPF_DEPENDENCIES OR (DEFINED _IBPF_PRIVATE_DEPENDENCIES AND _need_private_deps))
    set(PACKAGE_DEPENDENCIES "#### Expanded from @PACKAGE_DEPENDENCIES@ by install_basic_package_files() ####\n\ninclude(CMakeFindDependencyMacro)\n")

    foreach(_dep ${_IBPF_DEPENDENCIES})
      string(APPEND PACKAGE_DEPENDENCIES "find_dependency(${_dep})\n")
    endforeach()

    if(_need_private_deps)
      foreach(_dep ${_IBPF_PRIVATE_DEPENDENCIES})
        string(APPEND PACKAGE_DEPENDENCIES "find_dependency(${_dep})\n")
      endforeach()
    endif()

    string(APPEND PACKAGE_DEPENDENCIES "\n###############################################################################\n")
  endif()

  # Prepare PACKAGE_VERSION variable
  set(PACKAGE_VERSION ${_IBPF_VERSION})

  # <Name>Config.cmake (build tree)
  foreach(p ${_build_path_vars_suffix})
    if(DEFINED ${_IBPF_VARS_PREFIX}_BUILD_${p})
      set(${_IBPF_VARS_PREFIX}_${p} "${${_IBPF_VARS_PREFIX}_BUILD_${p}}")
    elseif(DEFINED BUILD_${_IBPF_VARS_PREFIX}_${p})
      set(${_IBPF_VARS_PREFIX}_${p} "${BUILD_${_IBPF_VARS_PREFIX}_${p}}")
    endif()
  endforeach()
  configure_package_config_file("${_config_cmake_in}"
                                "${_IBPF_EXPORT_DESTINATION}/${_config_filename}"
                                INSTALL_DESTINATION ${_IBPF_EXPORT_DESTINATION}
                                PATH_VARS ${_build_path_vars}
                                ${configure_package_config_file_extra_args}
                                INSTALL_PREFIX ${CMAKE_BINARY_DIR})

  # <Name>Config.cmake (installed)
  foreach(p ${_install_path_vars_suffix})
    if(DEFINED ${_IBPF_VARS_PREFIX}_INSTALL_${p})
      set(${_IBPF_VARS_PREFIX}_${p} "${${_IBPF_VARS_PREFIX}_INSTALL_${p}}")
    elseif(DEFINED INSTALL_${_IBPF_VARS_PREFIX}_${p})
      set(${_IBPF_VARS_PREFIX}_${p} "${INSTALL_${_IBPF_VARS_PREFIX}_${p}}")
    endif()
  endforeach()
  configure_package_config_file("${_config_cmake_in}"
                                "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_config_filename}.install"
                                INSTALL_DESTINATION ${_IBPF_INSTALL_DESTINATION}
                                PATH_VARS ${_install_path_vars}
                                ${configure_package_config_file_extra_args})
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_config_filename}.install"
          DESTINATION ${_IBPF_INSTALL_DESTINATION}
          RENAME ${_config_filename}
          COMPONENT ${_IBPF_COMPONENT})


  # <Name>Targets.cmake (build tree)
  if(NOT _IBPF_NO_EXPORT)
    export(${_export_cmd}
           NAMESPACE ${_IBPF_NAMESPACE}
           FILE "${_IBPF_EXPORT_DESTINATION}/${_targets_filename}")
  endif()

  # Export build directory if CMAKE_EXPORT_PACKAGE_REGISTRY is set.
  # CMake >= 3.15 already checks for CMAKE_EXPORT_PACKAGE_REGISTRY in `export(PACKAGE)` (cf.
  # cf. https://cmake.org/cmake/help/latest/policy/CMP0090.html), and we effectively back-port
  # this behavior to earlier versions.
  # Note that even never CMake versions may apply old policy behaviors if the consuming project
  # requires a lower version of CMake (e.g. `cmake_minimum_required(VERSION 3.14)`), so the
  # check for `CMAKE_EXPORT_PACKAGE_REGISTRY` is necessary for CMake >= 3.15 as well.
  if(CMAKE_EXPORT_PACKAGE_REGISTRY)
    export(PACKAGE ${_Name})
  endif()

  # <Name>Targets.cmake (installed)
  if(NOT _IBPF_NO_EXPORT)
    install(${_install_cmd}
            NAMESPACE ${_IBPF_NAMESPACE}
            DESTINATION ${_IBPF_INSTALL_DESTINATION}
            FILE "${_targets_filename}"
            COMPONENT ${_IBPF_COMPONENT})
  endif()
endfunction()
