/*
Copyright 2010 Filippo Erik Negroni <f.e.negroni@gmail.com>

This file is part of Sugo.

Sugo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Sugo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Sugo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h> // *printf,
#include <stdlib.h> // abort, exit,
#include <argp.h> // argp*,
#include <string.h> // basename (GNU), 
#include <unistd.h> // fork, pid_t,
#include <sys/types.h> // wait,
#include <sys/wait.h> // wait,
#include "test.h" // struct test,
#include "tests.h" // tests, init_tests, struct tests_list_item,

const int EXIT_ARGS = 1;

const char *argp_program_version = "0.1";
const char *argp_program_bug_address = "<f.e.negroni@gmail.com>";
error_t argp_err_exit_status;
const char *const NONPOSITIONAL_ARGS_DOC = "TEST...";
const char *const PROGRAM_DOC = "sugo -- a test framework for system routines.";

unsigned int verbosity_level = 1;

error_t
program_options_parser(int key, char *arg, struct argp_state *state)
{
	if ('v' == key) {
		++verbosity_level;
	} else if ('q' == key) {
		verbosity_level = 0;
	} else if (ARGP_KEY_ARG == key) {
		add_pending_test(arg);
	} else if (ARGP_KEY_END == key) {
		if (0 == pending_tests.count) {
			argp_error(state, "no tests specified.");
		}
	} else {
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

void
parse_args(int argc, char **argv)
{
	argp_err_exit_status = EXIT_ARGS;

	struct argp_option program_options[] = {
		{.name="verbose", .key='v', .arg=0, .flags=0, .doc="Make logging more verbose."},
		{.name="quiet", .key='q', .arg=0, .flags=0, .doc="Turn logging off completely."},
		{.name=0}
	};

	struct argp program_parser = {
		.options = program_options,
		.parser = program_options_parser,
		.args_doc = NONPOSITIONAL_ARGS_DOC,
		.doc = PROGRAM_DOC
	};

	error_t err = argp_parse(&program_parser, argc, argv, 0, 0, 0);
	if (err) {
		if (EINVAL == err) {
			exit(EXIT_ARGS);
		}
		abort();
	}
}

void
spawn_tests(void)
{
	struct test *t;

	while ((t = pop_pending_test())) {
		fprintf(stderr, "Spawning test %s: ", t->path);
		pid_t pid = fork();
		if (pid == 0) {
			/* Create new file descriptors for test: err and out, and input! */
			execl(t->path, basename(t->path), (void *)0);
			fprintf(stderr, "error: failed to exec %s\n", t->path);
			abort();
		} else if (-1 == pid) {
			fprintf(stderr, "error: unable to fork new test.\n");
			return;
		} else {
			t->pid = pid;
			test_is_running(t);
			fprintf(stderr, "pid %d\n", t->pid);
		}
	}
	all_tests_are_running();
}

void
wait_for_tests(void)
{
	while (running_tests.count) {
		int status;
		pid_t pid = wait(&status);
		test_completed(pid, status);
	}
}

void
process_tests(void)
{
	spawn_tests();
	wait_for_tests();
	//print_stats();
}

int
main(int argc, char **argv)
{
	init_tests_module();
	parse_args(argc, argv);
	finished_adding_pending_tests();
	process_tests();

	return 0;
}
