# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft

# Include any dependencies generated for this target.
include CMakeFiles/openal-info.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/openal-info.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/openal-info.dir/flags.make

CMakeFiles/openal-info.dir/utils/openal-info.o: CMakeFiles/openal-info.dir/flags.make
CMakeFiles/openal-info.dir/utils/openal-info.o: utils/openal-info.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/openal-info.dir/utils/openal-info.o"
	/home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/gcc-android  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/openal-info.dir/utils/openal-info.o   -c /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/utils/openal-info.c

CMakeFiles/openal-info.dir/utils/openal-info.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/openal-info.dir/utils/openal-info.i"
	/home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/gcc-android  $(C_DEFINES) $(C_FLAGS) -E /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/utils/openal-info.c > CMakeFiles/openal-info.dir/utils/openal-info.i

CMakeFiles/openal-info.dir/utils/openal-info.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/openal-info.dir/utils/openal-info.s"
	/home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/gcc-android  $(C_DEFINES) $(C_FLAGS) -S /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/utils/openal-info.c -o CMakeFiles/openal-info.dir/utils/openal-info.s

CMakeFiles/openal-info.dir/utils/openal-info.o.requires:
.PHONY : CMakeFiles/openal-info.dir/utils/openal-info.o.requires

CMakeFiles/openal-info.dir/utils/openal-info.o.provides: CMakeFiles/openal-info.dir/utils/openal-info.o.requires
	$(MAKE) -f CMakeFiles/openal-info.dir/build.make CMakeFiles/openal-info.dir/utils/openal-info.o.provides.build
.PHONY : CMakeFiles/openal-info.dir/utils/openal-info.o.provides

CMakeFiles/openal-info.dir/utils/openal-info.o.provides.build: CMakeFiles/openal-info.dir/utils/openal-info.o
.PHONY : CMakeFiles/openal-info.dir/utils/openal-info.o.provides.build

# Object files for target openal-info
openal__info_OBJECTS = \
"CMakeFiles/openal-info.dir/utils/openal-info.o"

# External object files for target openal-info
openal__info_EXTERNAL_OBJECTS =

openal-info: CMakeFiles/openal-info.dir/utils/openal-info.o
openal-info: libopenal.so.1.12
openal-info: CMakeFiles/openal-info.dir/build.make
openal-info: CMakeFiles/openal-info.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable openal-info"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/openal-info.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/openal-info.dir/build: openal-info
.PHONY : CMakeFiles/openal-info.dir/build

CMakeFiles/openal-info.dir/requires: CMakeFiles/openal-info.dir/utils/openal-info.o.requires
.PHONY : CMakeFiles/openal-info.dir/requires

CMakeFiles/openal-info.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/openal-info.dir/cmake_clean.cmake
.PHONY : CMakeFiles/openal-info.dir/clean

CMakeFiles/openal-info.dir/depend:
	cd /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft /home/onaips/workspace/supertux2/jni/openal/OpenAL-Soft/CMakeFiles/openal-info.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/openal-info.dir/depend

