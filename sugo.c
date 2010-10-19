/*
sugo -- a system unit test framework.

Copyright (C) 2010 Filippo Erik Negroni <f.e.negroni@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h> // fprintf, 
#include <stdlib.h> // abort, exit,
#include <argp.h> // argp*,
#include <string.h> // strdup, basename (GNU), 
#include <unistd.h> // fork,
#include <sys/types.h> // wait,
#include <sys/wait.h> // wait,

const int EXIT_ARGS = 1;

const char *argp_program_version = "0.1";
const char *argp_program_bug_address = "<f.e.negroni@gmail.com>";
error_t argp_err_exit_status;
const char *const NONPOSITIONAL_ARGS_DOC = "TEST...";
const char *const PROGRAM_DOC = "sugo -- a test framework for system routines.";

unsigned int verbosity_level = 1;

struct test {
	char *path;
	pid_t pid;
};

struct test_list_item {
	struct test test;
	struct test_list_item *next;
};

struct test_list {
	size_t count;
	struct test_list_item head, tail, *last;
};

struct test_list tests;

struct test_list_item *
new_test_list_item(const char *path)
{
	struct test_list_item *item = malloc(sizeof(struct test_list_item));
	item->test.path = strdup(path);
	item->test.pid = 0;
	return item;
}

void
append_test(const char *path)
{
	tests.last = tests.last->next = new_test_list_item(path);
	tests.last->next = &tests.tail;
	++tests.count;
}

struct test_list_item *
pop_test(pid_t pid)
{
	struct test_list_item *i = tests.head.next, *p = &tests.head;
	while (i != &tests.tail) {
		if (pid == i->test.pid) {
			p->next = i->next;
			--tests.count;
			break;
		}
		i = i->next;
	}
	return i;
}

error_t
program_options_parser(int key, char *arg, struct argp_state *state)
{
	if ('v' == key) {
		++verbosity_level;
	} else if ('q' == key) {
		verbosity_level = 0;
	} else if (ARGP_KEY_ARG == key) {
		append_test(arg);
	} else if (ARGP_KEY_END == key) {
		if (!tests.count) {
			argp_error(state, "no tests specified.");
		}
	} else {
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

void
init_tests(void)
{
	tests.count = 0;
	tests.head.next = &tests.tail;
	tests.tail.next = &tests.tail;
	tests.last = &tests.head;
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
run_tests(void)
{
	for (struct test_list_item *i = tests.head.next; i != &tests.tail; i = i->next) {
		printf("Spawning test %s: ", i->test.path);
		pid_t pid = fork();
		if (pid == 0) {
			/* Create new file descriptors for test: err and out, and input! */
			execl(i->test.path, basename(i->test.path), (void *)0);
			printf("Failed to exec %s\n", i->test.path);
			abort();
		} else if (-1 == pid) {
			printf("fatal error: unable to fork\n");
			break; // Proceed to wait for running tests
		} else {
			i->test.pid = pid;
			printf("pid %d\n", i->test.pid);
		}
	}

	while (tests.count) {
		int status;
		pid_t pid = wait(&status);

		struct test_list_item *item = pop_test(pid);
		if (item == &tests.tail) {
			printf("Got notification for child not in list\n");
			abort();
		}
		// Tell user all about the process termination: signal, exit status etc...
		printf("test %s terminated: status %d\n", item->test.path, status);
	}

	/* Compile statistics and print */
}

int
main(int argc, char **argv)
{
	init_tests();
	parse_args(argc, argv);
	run_tests();

	return 0;
}
