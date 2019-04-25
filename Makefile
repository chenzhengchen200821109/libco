# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chenzheng/Documents/libco

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chenzheng/Documents/libco

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/local/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/chenzheng/Documents/libco/CMakeFiles /home/chenzheng/Documents/libco/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/chenzheng/Documents/libco/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named colib_static

# Build rule for target.
colib_static: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 colib_static
.PHONY : colib_static

# fast build rule for target.
colib_static/fast:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/build
.PHONY : colib_static/fast

#=============================================================================
# Target rules for targets named colib_shared

# Build rule for target.
colib_shared: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 colib_shared
.PHONY : colib_shared

# fast build rule for target.
colib_shared/fast:
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/build
.PHONY : colib_shared/fast

#=============================================================================
# Target rules for targets named example_cond

# Build rule for target.
example_cond: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_cond
.PHONY : example_cond

# fast build rule for target.
example_cond/fast:
	$(MAKE) -f CMakeFiles/example_cond.dir/build.make CMakeFiles/example_cond.dir/build
.PHONY : example_cond/fast

#=============================================================================
# Target rules for targets named example_closure

# Build rule for target.
example_closure: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_closure
.PHONY : example_closure

# fast build rule for target.
example_closure/fast:
	$(MAKE) -f CMakeFiles/example_closure.dir/build.make CMakeFiles/example_closure.dir/build
.PHONY : example_closure/fast

#=============================================================================
# Target rules for targets named example_copystack

# Build rule for target.
example_copystack: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_copystack
.PHONY : example_copystack

# fast build rule for target.
example_copystack/fast:
	$(MAKE) -f CMakeFiles/example_copystack.dir/build.make CMakeFiles/example_copystack.dir/build
.PHONY : example_copystack/fast

#=============================================================================
# Target rules for targets named example_thread

# Build rule for target.
example_thread: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_thread
.PHONY : example_thread

# fast build rule for target.
example_thread/fast:
	$(MAKE) -f CMakeFiles/example_thread.dir/build.make CMakeFiles/example_thread.dir/build
.PHONY : example_thread/fast

#=============================================================================
# Target rules for targets named example_echocli

# Build rule for target.
example_echocli: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_echocli
.PHONY : example_echocli

# fast build rule for target.
example_echocli/fast:
	$(MAKE) -f CMakeFiles/example_echocli.dir/build.make CMakeFiles/example_echocli.dir/build
.PHONY : example_echocli/fast

#=============================================================================
# Target rules for targets named example_poll

# Build rule for target.
example_poll: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_poll
.PHONY : example_poll

# fast build rule for target.
example_poll/fast:
	$(MAKE) -f CMakeFiles/example_poll.dir/build.make CMakeFiles/example_poll.dir/build
.PHONY : example_poll/fast

#=============================================================================
# Target rules for targets named example_setenv

# Build rule for target.
example_setenv: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_setenv
.PHONY : example_setenv

# fast build rule for target.
example_setenv/fast:
	$(MAKE) -f CMakeFiles/example_setenv.dir/build.make CMakeFiles/example_setenv.dir/build
.PHONY : example_setenv/fast

#=============================================================================
# Target rules for targets named example_echosvr

# Build rule for target.
example_echosvr: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_echosvr
.PHONY : example_echosvr

# fast build rule for target.
example_echosvr/fast:
	$(MAKE) -f CMakeFiles/example_echosvr.dir/build.make CMakeFiles/example_echosvr.dir/build
.PHONY : example_echosvr/fast

#=============================================================================
# Target rules for targets named example_specific

# Build rule for target.
example_specific: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 example_specific
.PHONY : example_specific

# fast build rule for target.
example_specific/fast:
	$(MAKE) -f CMakeFiles/example_specific.dir/build.make CMakeFiles/example_specific.dir/build
.PHONY : example_specific/fast

co_epoll.o: co_epoll.cpp.o

.PHONY : co_epoll.o

# target to build an object file
co_epoll.cpp.o:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_epoll.cpp.o
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_epoll.cpp.o
.PHONY : co_epoll.cpp.o

co_epoll.i: co_epoll.cpp.i

.PHONY : co_epoll.i

# target to preprocess a source file
co_epoll.cpp.i:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_epoll.cpp.i
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_epoll.cpp.i
.PHONY : co_epoll.cpp.i

co_epoll.s: co_epoll.cpp.s

.PHONY : co_epoll.s

# target to generate assembly for a file
co_epoll.cpp.s:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_epoll.cpp.s
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_epoll.cpp.s
.PHONY : co_epoll.cpp.s

co_hook_sys_call.o: co_hook_sys_call.cpp.o

.PHONY : co_hook_sys_call.o

# target to build an object file
co_hook_sys_call.cpp.o:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_hook_sys_call.cpp.o
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_hook_sys_call.cpp.o
.PHONY : co_hook_sys_call.cpp.o

co_hook_sys_call.i: co_hook_sys_call.cpp.i

.PHONY : co_hook_sys_call.i

# target to preprocess a source file
co_hook_sys_call.cpp.i:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_hook_sys_call.cpp.i
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_hook_sys_call.cpp.i
.PHONY : co_hook_sys_call.cpp.i

co_hook_sys_call.s: co_hook_sys_call.cpp.s

.PHONY : co_hook_sys_call.s

# target to generate assembly for a file
co_hook_sys_call.cpp.s:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_hook_sys_call.cpp.s
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_hook_sys_call.cpp.s
.PHONY : co_hook_sys_call.cpp.s

co_routine.o: co_routine.cpp.o

.PHONY : co_routine.o

# target to build an object file
co_routine.cpp.o:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_routine.cpp.o
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_routine.cpp.o
.PHONY : co_routine.cpp.o

co_routine.i: co_routine.cpp.i

.PHONY : co_routine.i

# target to preprocess a source file
co_routine.cpp.i:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_routine.cpp.i
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_routine.cpp.i
.PHONY : co_routine.cpp.i

co_routine.s: co_routine.cpp.s

.PHONY : co_routine.s

# target to generate assembly for a file
co_routine.cpp.s:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/co_routine.cpp.s
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/co_routine.cpp.s
.PHONY : co_routine.cpp.s

coctx.o: coctx.cpp.o

.PHONY : coctx.o

# target to build an object file
coctx.cpp.o:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/coctx.cpp.o
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/coctx.cpp.o
.PHONY : coctx.cpp.o

coctx.i: coctx.cpp.i

.PHONY : coctx.i

# target to preprocess a source file
coctx.cpp.i:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/coctx.cpp.i
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/coctx.cpp.i
.PHONY : coctx.cpp.i

coctx.s: coctx.cpp.s

.PHONY : coctx.s

# target to generate assembly for a file
coctx.cpp.s:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/coctx.cpp.s
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/coctx.cpp.s
.PHONY : coctx.cpp.s

coctx_swap.o: coctx_swap.S.o

.PHONY : coctx_swap.o

# target to build an object file
coctx_swap.S.o:
	$(MAKE) -f CMakeFiles/colib_static.dir/build.make CMakeFiles/colib_static.dir/coctx_swap.S.o
	$(MAKE) -f CMakeFiles/colib_shared.dir/build.make CMakeFiles/colib_shared.dir/coctx_swap.S.o
.PHONY : coctx_swap.S.o

example_closure.o: example_closure.cpp.o

.PHONY : example_closure.o

# target to build an object file
example_closure.cpp.o:
	$(MAKE) -f CMakeFiles/example_closure.dir/build.make CMakeFiles/example_closure.dir/example_closure.cpp.o
.PHONY : example_closure.cpp.o

example_closure.i: example_closure.cpp.i

.PHONY : example_closure.i

# target to preprocess a source file
example_closure.cpp.i:
	$(MAKE) -f CMakeFiles/example_closure.dir/build.make CMakeFiles/example_closure.dir/example_closure.cpp.i
.PHONY : example_closure.cpp.i

example_closure.s: example_closure.cpp.s

.PHONY : example_closure.s

# target to generate assembly for a file
example_closure.cpp.s:
	$(MAKE) -f CMakeFiles/example_closure.dir/build.make CMakeFiles/example_closure.dir/example_closure.cpp.s
.PHONY : example_closure.cpp.s

example_cond.o: example_cond.cpp.o

.PHONY : example_cond.o

# target to build an object file
example_cond.cpp.o:
	$(MAKE) -f CMakeFiles/example_cond.dir/build.make CMakeFiles/example_cond.dir/example_cond.cpp.o
.PHONY : example_cond.cpp.o

example_cond.i: example_cond.cpp.i

.PHONY : example_cond.i

# target to preprocess a source file
example_cond.cpp.i:
	$(MAKE) -f CMakeFiles/example_cond.dir/build.make CMakeFiles/example_cond.dir/example_cond.cpp.i
.PHONY : example_cond.cpp.i

example_cond.s: example_cond.cpp.s

.PHONY : example_cond.s

# target to generate assembly for a file
example_cond.cpp.s:
	$(MAKE) -f CMakeFiles/example_cond.dir/build.make CMakeFiles/example_cond.dir/example_cond.cpp.s
.PHONY : example_cond.cpp.s

example_copystack.o: example_copystack.cpp.o

.PHONY : example_copystack.o

# target to build an object file
example_copystack.cpp.o:
	$(MAKE) -f CMakeFiles/example_copystack.dir/build.make CMakeFiles/example_copystack.dir/example_copystack.cpp.o
.PHONY : example_copystack.cpp.o

example_copystack.i: example_copystack.cpp.i

.PHONY : example_copystack.i

# target to preprocess a source file
example_copystack.cpp.i:
	$(MAKE) -f CMakeFiles/example_copystack.dir/build.make CMakeFiles/example_copystack.dir/example_copystack.cpp.i
.PHONY : example_copystack.cpp.i

example_copystack.s: example_copystack.cpp.s

.PHONY : example_copystack.s

# target to generate assembly for a file
example_copystack.cpp.s:
	$(MAKE) -f CMakeFiles/example_copystack.dir/build.make CMakeFiles/example_copystack.dir/example_copystack.cpp.s
.PHONY : example_copystack.cpp.s

example_echocli.o: example_echocli.cpp.o

.PHONY : example_echocli.o

# target to build an object file
example_echocli.cpp.o:
	$(MAKE) -f CMakeFiles/example_echocli.dir/build.make CMakeFiles/example_echocli.dir/example_echocli.cpp.o
.PHONY : example_echocli.cpp.o

example_echocli.i: example_echocli.cpp.i

.PHONY : example_echocli.i

# target to preprocess a source file
example_echocli.cpp.i:
	$(MAKE) -f CMakeFiles/example_echocli.dir/build.make CMakeFiles/example_echocli.dir/example_echocli.cpp.i
.PHONY : example_echocli.cpp.i

example_echocli.s: example_echocli.cpp.s

.PHONY : example_echocli.s

# target to generate assembly for a file
example_echocli.cpp.s:
	$(MAKE) -f CMakeFiles/example_echocli.dir/build.make CMakeFiles/example_echocli.dir/example_echocli.cpp.s
.PHONY : example_echocli.cpp.s

example_echosvr.o: example_echosvr.cpp.o

.PHONY : example_echosvr.o

# target to build an object file
example_echosvr.cpp.o:
	$(MAKE) -f CMakeFiles/example_echosvr.dir/build.make CMakeFiles/example_echosvr.dir/example_echosvr.cpp.o
.PHONY : example_echosvr.cpp.o

example_echosvr.i: example_echosvr.cpp.i

.PHONY : example_echosvr.i

# target to preprocess a source file
example_echosvr.cpp.i:
	$(MAKE) -f CMakeFiles/example_echosvr.dir/build.make CMakeFiles/example_echosvr.dir/example_echosvr.cpp.i
.PHONY : example_echosvr.cpp.i

example_echosvr.s: example_echosvr.cpp.s

.PHONY : example_echosvr.s

# target to generate assembly for a file
example_echosvr.cpp.s:
	$(MAKE) -f CMakeFiles/example_echosvr.dir/build.make CMakeFiles/example_echosvr.dir/example_echosvr.cpp.s
.PHONY : example_echosvr.cpp.s

example_poll.o: example_poll.cpp.o

.PHONY : example_poll.o

# target to build an object file
example_poll.cpp.o:
	$(MAKE) -f CMakeFiles/example_poll.dir/build.make CMakeFiles/example_poll.dir/example_poll.cpp.o
.PHONY : example_poll.cpp.o

example_poll.i: example_poll.cpp.i

.PHONY : example_poll.i

# target to preprocess a source file
example_poll.cpp.i:
	$(MAKE) -f CMakeFiles/example_poll.dir/build.make CMakeFiles/example_poll.dir/example_poll.cpp.i
.PHONY : example_poll.cpp.i

example_poll.s: example_poll.cpp.s

.PHONY : example_poll.s

# target to generate assembly for a file
example_poll.cpp.s:
	$(MAKE) -f CMakeFiles/example_poll.dir/build.make CMakeFiles/example_poll.dir/example_poll.cpp.s
.PHONY : example_poll.cpp.s

example_setenv.o: example_setenv.cpp.o

.PHONY : example_setenv.o

# target to build an object file
example_setenv.cpp.o:
	$(MAKE) -f CMakeFiles/example_setenv.dir/build.make CMakeFiles/example_setenv.dir/example_setenv.cpp.o
.PHONY : example_setenv.cpp.o

example_setenv.i: example_setenv.cpp.i

.PHONY : example_setenv.i

# target to preprocess a source file
example_setenv.cpp.i:
	$(MAKE) -f CMakeFiles/example_setenv.dir/build.make CMakeFiles/example_setenv.dir/example_setenv.cpp.i
.PHONY : example_setenv.cpp.i

example_setenv.s: example_setenv.cpp.s

.PHONY : example_setenv.s

# target to generate assembly for a file
example_setenv.cpp.s:
	$(MAKE) -f CMakeFiles/example_setenv.dir/build.make CMakeFiles/example_setenv.dir/example_setenv.cpp.s
.PHONY : example_setenv.cpp.s

example_specific.o: example_specific.cpp.o

.PHONY : example_specific.o

# target to build an object file
example_specific.cpp.o:
	$(MAKE) -f CMakeFiles/example_specific.dir/build.make CMakeFiles/example_specific.dir/example_specific.cpp.o
.PHONY : example_specific.cpp.o

example_specific.i: example_specific.cpp.i

.PHONY : example_specific.i

# target to preprocess a source file
example_specific.cpp.i:
	$(MAKE) -f CMakeFiles/example_specific.dir/build.make CMakeFiles/example_specific.dir/example_specific.cpp.i
.PHONY : example_specific.cpp.i

example_specific.s: example_specific.cpp.s

.PHONY : example_specific.s

# target to generate assembly for a file
example_specific.cpp.s:
	$(MAKE) -f CMakeFiles/example_specific.dir/build.make CMakeFiles/example_specific.dir/example_specific.cpp.s
.PHONY : example_specific.cpp.s

example_thread.o: example_thread.cpp.o

.PHONY : example_thread.o

# target to build an object file
example_thread.cpp.o:
	$(MAKE) -f CMakeFiles/example_thread.dir/build.make CMakeFiles/example_thread.dir/example_thread.cpp.o
.PHONY : example_thread.cpp.o

example_thread.i: example_thread.cpp.i

.PHONY : example_thread.i

# target to preprocess a source file
example_thread.cpp.i:
	$(MAKE) -f CMakeFiles/example_thread.dir/build.make CMakeFiles/example_thread.dir/example_thread.cpp.i
.PHONY : example_thread.cpp.i

example_thread.s: example_thread.cpp.s

.PHONY : example_thread.s

# target to generate assembly for a file
example_thread.cpp.s:
	$(MAKE) -f CMakeFiles/example_thread.dir/build.make CMakeFiles/example_thread.dir/example_thread.cpp.s
.PHONY : example_thread.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... colib_static"
	@echo "... colib_shared"
	@echo "... example_cond"
	@echo "... example_closure"
	@echo "... example_copystack"
	@echo "... example_thread"
	@echo "... example_echocli"
	@echo "... example_poll"
	@echo "... example_setenv"
	@echo "... example_echosvr"
	@echo "... example_specific"
	@echo "... co_epoll.o"
	@echo "... co_epoll.i"
	@echo "... co_epoll.s"
	@echo "... co_hook_sys_call.o"
	@echo "... co_hook_sys_call.i"
	@echo "... co_hook_sys_call.s"
	@echo "... co_routine.o"
	@echo "... co_routine.i"
	@echo "... co_routine.s"
	@echo "... coctx.o"
	@echo "... coctx.i"
	@echo "... coctx.s"
	@echo "... coctx_swap.o"
	@echo "... example_closure.o"
	@echo "... example_closure.i"
	@echo "... example_closure.s"
	@echo "... example_cond.o"
	@echo "... example_cond.i"
	@echo "... example_cond.s"
	@echo "... example_copystack.o"
	@echo "... example_copystack.i"
	@echo "... example_copystack.s"
	@echo "... example_echocli.o"
	@echo "... example_echocli.i"
	@echo "... example_echocli.s"
	@echo "... example_echosvr.o"
	@echo "... example_echosvr.i"
	@echo "... example_echosvr.s"
	@echo "... example_poll.o"
	@echo "... example_poll.i"
	@echo "... example_poll.s"
	@echo "... example_setenv.o"
	@echo "... example_setenv.i"
	@echo "... example_setenv.s"
	@echo "... example_specific.o"
	@echo "... example_specific.i"
	@echo "... example_specific.s"
	@echo "... example_thread.o"
	@echo "... example_thread.i"
	@echo "... example_thread.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

