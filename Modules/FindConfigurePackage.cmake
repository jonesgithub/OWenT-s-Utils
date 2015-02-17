#.rst:
# FindConfigurePackage
# ----------------
#
# find package, and try to configure it when not found in system.
#
# FindConfigurePackage(
#     PACKAGE <name>
#     BUILD_WITH_CONFIGURE
#     BUILD_WITH_CMAKE
#     CONFIGURE_FLAGS [configure options...]
#     CMAKE_FLAGS [cmake options...]
#     MAKE_FLAGS [make options...]
#     WORKING_DIRECTORY <work directory>
#     BUILD_DIRECTORY <build directory>
#     PREFIX_DIRECTORY <prefix directory>
#     SRC_DIRECTORY_NAME <source directory name>
#     ZIP_URL <zip url>
#     TAR_URL <tar url>
#     SVN_URL <svn url>
#     GIT_URL <git url>
# )
#
# ::
#
#   <configure options>     - flags added to configure command
#   <cmake options>         - flags added to cmake command
#   <make options>          - flags added to make command
#   <work directory>        - work directory
#   <build directory>       - where to execute configure and make
#   <prefix directory>      - prefix directory(default: <work directory>)
#   <source directory name> - source directory name(default detected by download url)
#   <zip url>               - from where to download zip when find package failed
#   <tar url>               - from where to download tar.* when find package failed
#   <svn url>               - from where to svn co when find package failed
#   <git url>               - from where to git clone when find package failed
#

#=============================================================================
# Copyright 2014-2015 OWenT.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)


macro (FindConfigurePackage)
    include(CMakeParseArguments)
    set(optionArgs BUILD_WITH_CONFIGURE BUILD_WITH_CMAKE)
    set(oneValueArgs PACKAGE WORKING_DIRECTORY BUILD_DIRECTORY PREFIX_DIRECTORY SRC_DIRECTORY_NAME ZIP_URL TAR_URL SVN_URL GIT_URL)
    set(multiValueArgs CONFIGURE_CMD CONFIGURE_FLAGS CMAKE_FLAGS MAKE_FLAGS)
    cmake_parse_arguments(FindConfigurePackage "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # step 1. find using standard method
    find_package(${FindConfigurePackage_PACKAGE})
    if(NOT ${FindConfigurePackage_PACKAGE}_FOUND)
        if(NOT FindConfigurePackage_PREFIX_DIRECTORY)
            # prefix
            set(FindConfigurePackage_PREFIX_DIRECTORY ${FindConfigurePackage_WORK_DIRECTORY})
        endif()

        # set package root
        set(${FindConfigurePackage_PACKAGE}_ROOT "${FindConfigurePackage_PREFIX_DIRECTORY}")
        # set package root upper case
        string(TOUPPER "${FindConfigurePackage_PACKAGE}_ROOT" ${FindConfigurePackage_PACKAGE}_ROOT_UPPER)
        set(${FindConfigurePackage_PACKAGE}_ROOT_UPPER "${FindConfigurePackage_PREFIX_DIRECTORY}")

        # step 2. find in prefix
        find_package(${FindConfigurePackage_PACKAGE})

        # step 3. build
        if(NOT ${FindConfigurePackage_PACKAGE}_FOUND)
            # zip package
            if(FindConfigurePackage_ZIP_URL)
                get_filename_component(DOWNLOAD_FILENAME "${FindConfigurePackage_ZIP_URL}" NAME)

                if(NOT FindConfigurePackage_SRC_DIRECTORY_NAME)
                    string(REGEX REPLACE "\\.zip$" "" FindConfigurePackage_SRC_DIRECTORY_NAME "${DOWNLOAD_FILENAME}")
                endif()

                if(NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}")
                    message(STATUS "start to download ${DOWNLOAD_FILENAME} from ${FindConfigurePackage_ZIP_URL}")
                    file(DOWNLOAD "${FindConfigurePackage_ZIP_URL}" "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}" SHOW_PROGRESS)
                endif()

                find_program(ZIP_EXECUTABLE wzzip PATHS "$ENV{ProgramFiles}/WinZip")
                if(ZIP_EXECUTABLE AND NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                    execute_process(COMMAND ${ZIP_EXECUTABLE} -f "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}"
                        WORKING_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}"
                    )
                endif()

                if(NOT ZIP_EXECUTABLE AND NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                  find_program(ZIP_EXECUTABLE 7z PATHS "$ENV{ProgramFiles}/7-Zip")
                  if(ZIP_EXECUTABLE)
                      execute_process(COMMAND ${ZIP_EXECUTABLE} -r -y "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}"
                          WORKING_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}"
                      )
                  endif()
                endif()

                if(NOT ZIP_EXECUTABLE AND NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                  find_package(Cygwin)
                  find_program(ZIP_EXECUTABLE unzip PATHS "${CYGWIN_INSTALL_PATH}/bin")
                  if(ZIP_EXECUTABLE)
                      execute_process(COMMAND ${ZIP_EXECUTABLE} -f "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}"
                          WORKING_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}"
                      )
                  endif()
                endif()

                if(NOT ZIP_EXECUTABLE)
                    message(FATAL_ERROR "unzip tools not found")
                endif()

            # tar package
            elseif(FindConfigurePackage_TAR_URL)
                get_filename_component(DOWNLOAD_FILENAME "${FindConfigurePackage_TAR_URL}" NAME)
                if(NOT FindConfigurePackage_SRC_DIRECTORY_NAME)
                    string(REGEX REPLACE "\\.tar\\.[A-Za-z]$" "" FindConfigurePackage_SRC_DIRECTORY_NAME "${DOWNLOAD_FILENAME}")
                endif()

                if(NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}")
                    message(STATUS "start to download ${DOWNLOAD_FILENAME} from ${FindConfigurePackage_TAR_URL}")
                    file(DOWNLOAD "${FindConfigurePackage_TAR_URL}" "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}" SHOW_PROGRESS)
                endif()

                find_program(TAR_EXECUTABLE tar PATHS "${CYGWIN_INSTALL_PATH}/bin")
                if(TAR_EXECUTABLE AND NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                    execute_process(COMMAND ${TAR_EXECUTABLE} -axvf "${FindConfigurePackage_WORKING_DIRECTORY}/${DOWNLOAD_FILENAME}"
                        WORKING_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}"
                    )
                endif()

                if(NOT TAR_EXECUTABLE)
                    message(FATAL_ERROR "tar not found")
                endif()

            # git package
            elseif(FindConfigurePackage_GIT_URL)
                get_filename_component(DOWNLOAD_FILENAME "${FindConfigurePackage_GIT_URL}" NAME)
                if(NOT FindConfigurePackage_SRC_DIRECTORY_NAME)
                    get_filename_component(FindConfigurePackage_SRC_DIRECTORY_FULL_NAME "${DOWNLOAD_FILENAME}" NAME)
                    string(REGEX REPLACE "\\.git$" "" FindConfigurePackage_SRC_DIRECTORY_NAME "${FindConfigurePackage_SRC_DIRECTORY_FULL_NAME}")
                endif()

                if(NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                    find_package(Git)
                    if(NOT GIT_FOUND)
                       message(FATAL_ERROR "git not found")
                    endif()

                    execute_process(COMMAND ${GIT_EXECUTABLE} clone "${FindConfigurePackage_GIT_URL}" "${FindConfigurePackage_SRC_DIRECTORY_NAME}"
                        WORKING_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}"
                    )
                endif()

            # svn package
            elseif(FindConfigurePackage_SVN_URL)
                get_filename_component(DOWNLOAD_FILENAME "${FindConfigurePackage_SVN_URL}" NAME)

                if(NOT FindConfigurePackage_SRC_DIRECTORY_NAME)
                    get_filename_component(FindConfigurePackage_SRC_DIRECTORY_NAME "${DOWNLOAD_FILENAME}" NAME)
                endif()

                if(NOT EXISTS "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                    find_package(Subversion)
                    if(NOT SUBVERSION_FOUND)
                       message(FATAL_ERROR "svn not found")
                    endif()

                    execute_process(COMMAND ${Subversion_SVN_EXECUTABLE} co "${FindConfigurePackage_SVN_URL}" "${FindConfigurePackage_SRC_DIRECTORY_NAME}"
                        WORKING_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}"
                    )
                endif()
            endif()

            # build using configure and make
            if (NOT FindConfigurePackage_BUILD_DIRECTORY)
                set(FindConfigurePackage_BUILD_DIRECTORY "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
            endif()

            if(FindConfigurePackage_BUILD_WITH_CONFIGURE)
                file(RELATIVE_PATH CONFIGURE_EXEC_FILE ${FindConfigurePackage_BUILD_DIRECTORY} "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}/configure")
                if ( ${CONFIGURE_EXEC_FILE} STREQUAL "configure")
                    set(CONFIGURE_EXEC_FILE "./configure")
                endif()
                execute_process(
                    COMMAND ${CONFIGURE_EXEC_FILE} "--prefix=${FindConfigurePackage_PREFIX_DIRECTORY}" ${FindConfigurePackage_CONFIGURE_FLAGS}
                    WORKING_DIRECTORY "${FindConfigurePackage_BUILD_DIRECTORY}"
                )

                execute_process(
                    COMMAND "make" ${FindConfigurePackage_MAKE_FLAGS} "install"
                    WORKING_DIRECTORY "${FindConfigurePackage_BUILD_DIRECTORY}"
                )

            elseif(FindConfigurePackage_BUILD_WITH_CMAKE)
                file(RELATIVE_PATH BUILD_WITH_CMAKE_PROJECT_DIR ${FindConfigurePackage_BUILD_DIRECTORY} "${FindConfigurePackage_WORKING_DIRECTORY}/${FindConfigurePackage_SRC_DIRECTORY_NAME}")
                if ( NOT BUILD_WITH_CMAKE_PROJECT_DIR)
                    set(BUILD_WITH_CMAKE_PROJECT_DIR ".")
                endif()
                execute_process(
                    COMMAND "cmake" ${BUILD_WITH_CMAKE_PROJECT_DIR} "-DCMAKE_INSTALL_PREFIX=${FindConfigurePackage_PREFIX_DIRECTORY}" ${FindConfigurePackage_CMAKE_FLAGS}
                    WORKING_DIRECTORY "${FindConfigurePackage_BUILD_DIRECTORY}"
                )

                execute_process(
                    COMMAND "make" ${FindConfigurePackage_MAKE_FLAGS} "install"
                    WORKING_DIRECTORY "${FindConfigurePackage_BUILD_DIRECTORY}"
                )
            endif()

            find_package(${FindConfigurePackage_PACKAGE})
        endif()
    endif()
endmacro(FindConfigurePackage)


