# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.21.1/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.21.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vas/repos/matemancpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vas/repos/matemancpp

# Include any dependencies generated for this target.
include src/CMakeFiles/matemancpp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/matemancpp.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/matemancpp.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/matemancpp.dir/flags.make

src/CMakeFiles/matemancpp.dir/main.cpp.o: src/CMakeFiles/matemancpp.dir/flags.make
src/CMakeFiles/matemancpp.dir/main.cpp.o: src/main.cpp
src/CMakeFiles/matemancpp.dir/main.cpp.o: src/CMakeFiles/matemancpp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/vas/repos/matemancpp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/matemancpp.dir/main.cpp.o"
	cd /Users/vas/repos/matemancpp/src && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/matemancpp.dir/main.cpp.o -MF CMakeFiles/matemancpp.dir/main.cpp.o.d -o CMakeFiles/matemancpp.dir/main.cpp.o -c /Users/vas/repos/matemancpp/src/main.cpp

src/CMakeFiles/matemancpp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/matemancpp.dir/main.cpp.i"
	cd /Users/vas/repos/matemancpp/src && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vas/repos/matemancpp/src/main.cpp > CMakeFiles/matemancpp.dir/main.cpp.i

src/CMakeFiles/matemancpp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/matemancpp.dir/main.cpp.s"
	cd /Users/vas/repos/matemancpp/src && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vas/repos/matemancpp/src/main.cpp -o CMakeFiles/matemancpp.dir/main.cpp.s

# Object files for target matemancpp
matemancpp_OBJECTS = \
"CMakeFiles/matemancpp.dir/main.cpp.o"

# External object files for target matemancpp
matemancpp_EXTERNAL_OBJECTS =

src/matemancpp: src/CMakeFiles/matemancpp.dir/main.cpp.o
src/matemancpp: src/CMakeFiles/matemancpp.dir/build.make
src/matemancpp: /opt/homebrew/lib/libspdlog.1.9.2.dylib
src/matemancpp: /usr/local/lib/libCatch2.a
src/matemancpp: src/libmatemancpp_lib.a
src/matemancpp: /opt/homebrew/lib/libfmt.8.0.1.dylib
src/matemancpp: src/CMakeFiles/matemancpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/vas/repos/matemancpp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable matemancpp"
	cd /Users/vas/repos/matemancpp/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/matemancpp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/matemancpp.dir/build: src/matemancpp
.PHONY : src/CMakeFiles/matemancpp.dir/build

src/CMakeFiles/matemancpp.dir/clean:
	cd /Users/vas/repos/matemancpp/src && $(CMAKE_COMMAND) -P CMakeFiles/matemancpp.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/matemancpp.dir/clean

src/CMakeFiles/matemancpp.dir/depend:
	cd /Users/vas/repos/matemancpp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vas/repos/matemancpp /Users/vas/repos/matemancpp/src /Users/vas/repos/matemancpp /Users/vas/repos/matemancpp/src /Users/vas/repos/matemancpp/src/CMakeFiles/matemancpp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/matemancpp.dir/depend
