# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/huan/projects/ChatServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/huan/projects/ChatServer/build

# Include any dependencies generated for this target.
include src/client/CMakeFiles/ChatClient.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/client/CMakeFiles/ChatClient.dir/compiler_depend.make

# Include the progress variables for this target.
include src/client/CMakeFiles/ChatClient.dir/progress.make

# Include the compile flags for this target's objects.
include src/client/CMakeFiles/ChatClient.dir/flags.make

src/client/CMakeFiles/ChatClient.dir/main.cpp.o: src/client/CMakeFiles/ChatClient.dir/flags.make
src/client/CMakeFiles/ChatClient.dir/main.cpp.o: /home/huan/projects/ChatServer/src/client/main.cpp
src/client/CMakeFiles/ChatClient.dir/main.cpp.o: src/client/CMakeFiles/ChatClient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/huan/projects/ChatServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/client/CMakeFiles/ChatClient.dir/main.cpp.o"
	cd /home/huan/projects/ChatServer/build/src/client && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/client/CMakeFiles/ChatClient.dir/main.cpp.o -MF CMakeFiles/ChatClient.dir/main.cpp.o.d -o CMakeFiles/ChatClient.dir/main.cpp.o -c /home/huan/projects/ChatServer/src/client/main.cpp

src/client/CMakeFiles/ChatClient.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ChatClient.dir/main.cpp.i"
	cd /home/huan/projects/ChatServer/build/src/client && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/huan/projects/ChatServer/src/client/main.cpp > CMakeFiles/ChatClient.dir/main.cpp.i

src/client/CMakeFiles/ChatClient.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ChatClient.dir/main.cpp.s"
	cd /home/huan/projects/ChatServer/build/src/client && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/huan/projects/ChatServer/src/client/main.cpp -o CMakeFiles/ChatClient.dir/main.cpp.s

# Object files for target ChatClient
ChatClient_OBJECTS = \
"CMakeFiles/ChatClient.dir/main.cpp.o"

# External object files for target ChatClient
ChatClient_EXTERNAL_OBJECTS =

/home/huan/projects/ChatServer/bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/main.cpp.o
/home/huan/projects/ChatServer/bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/build.make
/home/huan/projects/ChatServer/bin/ChatClient: src/client/CMakeFiles/ChatClient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/huan/projects/ChatServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/huan/projects/ChatServer/bin/ChatClient"
	cd /home/huan/projects/ChatServer/build/src/client && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ChatClient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/client/CMakeFiles/ChatClient.dir/build: /home/huan/projects/ChatServer/bin/ChatClient
.PHONY : src/client/CMakeFiles/ChatClient.dir/build

src/client/CMakeFiles/ChatClient.dir/clean:
	cd /home/huan/projects/ChatServer/build/src/client && $(CMAKE_COMMAND) -P CMakeFiles/ChatClient.dir/cmake_clean.cmake
.PHONY : src/client/CMakeFiles/ChatClient.dir/clean

src/client/CMakeFiles/ChatClient.dir/depend:
	cd /home/huan/projects/ChatServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/huan/projects/ChatServer /home/huan/projects/ChatServer/src/client /home/huan/projects/ChatServer/build /home/huan/projects/ChatServer/build/src/client /home/huan/projects/ChatServer/build/src/client/CMakeFiles/ChatClient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/client/CMakeFiles/ChatClient.dir/depend

