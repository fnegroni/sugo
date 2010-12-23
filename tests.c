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

#include <string.h> // strdup,
#include <unistd.h> // test.h,
#include "test.h"

struct tests_list_item {
	struct test *test;
	struct tests_item *next;
};

struct tests_list {
	size_t count;
	struct test_list_item head, *last;
};

struct tests_list_iterator {
	struct test_list_item *i;
};

typedef tests_list tests_t;
typedef tests_list_item tests_item_t;
typedef tests_list_iterator tests_iterator_t;

tests_t tests;

void
init_tests(void)
{
	tests.count = 0;
	tests.head.next = 0;
	tests.last = &tests.head;
}

struct test *
new_test(const char *path)
{
	struct test *test = malloc(sizeof(struct test));
	test->path = strdup(path);
	test->pid = 0;
	return test;
}

struct test_list_item *
new_test_list_item(const char *path)
{
	struct test_list_item *item = malloc(sizeof(struct test_list_item));
	item->test = new_test(path);
	return item;
}

void
append_test(const char *path)
{
	tests.last = tests.last->next = new_test_list_item(path);
	tests.last->next = 0;
	++tests.count;
}

struct test *
pop_test(pid_t pid)
{
	struct test_list_item *p, *i;
	for (p = &tests.head, i = p->next; i; p = i, i = p->next) {
		if (pid == i->test->pid) {
			p->next = i->next;
			--tests.count;
			return i->test;
		}
	}
	return 0;
}

struct test *
get_test_from_tests_item(struct test_list_item *i)
{
	if (!i) return 0;
	return i->test;
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

struct tests_iterator {
	struct test_list_item *p;
};

struct test *
get_first_test(struct tests_iterator *it)
{
	it->p = tests.head;
	return get_next_test(it);
}

struct test *
get_next_test(struct tests_iterator *it)
{
	it->p = it->p->next;
	return get_test(it->p);
}

void
run_tests(void)
{
	struct tests_iterator it;
	for (struct test *i = get_first_test(&it); i; i = get_next_test(iterator)) {
	}

	for (struct test_list_item *i = tests.head.next; i; i = i->next) {
		printf("Spawning test %s: ", i->test->path);
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

		// Should use tests here
		struct test_list_item *i = pop_test(pid);
		// Every time we pop a test, we add it to another list, which records it as ended, so we can collect statistics.
		// We should have two lists: the lists of running tests and the list of all tests
		// Running tests list shrinks, total tests stay same.
		// This way we can collect statistics.
		if (!i) {
			printf("Got notification for child not in list\n");
			abort();
		}
		// Tell user all about the process termination: signal, exit status etc...
		printf("test %s terminated: status %d\n", i->test.path, status);
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
