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

#include <unistd.h> // test.h,
#include "test.h" // tests.h,
#include "tests.h"
#include <stdlib.h> // malloc,
#include <assert.h> // assert,
#include <string.h> // memmove,
#include <stdio.h> // fprintf,

struct tests_queue pending_tests;
struct tests_vector running_tests;
struct tests_vector completed_tests;

void
init_tests_module(void)
{
	pending_tests.count = 0;
	pending_tests.ph.test = 0;
	pending_tests.ph.next = &pending_tests.ph;
	pending_tests.front = pending_tests.back = &pending_tests.ph.next;
	running_tests.count = 0;
	running_tests.array = 0;
	running_tests.front = running_tests.array;
	completed_tests.count = 0;
	completed_tests.array = 0;
	completed_tests.front = completed_tests.array;
}

void
add_pending_test(const char *path)
{
	*pending_tests.back = malloc(sizeof **pending_tests.back);
	(*pending_tests.back)->next = &pending_tests.ph;
	(*pending_tests.back)->test = new_test(path);
	pending_tests.back = &(*pending_tests.back)->next;
	++pending_tests.count;
}

struct test *
next_pending_test(void)
{
	struct test *t = (*pending_tests.front)->test;
	pending_tests.front = &(*pending_tests.front)->next;
	--pending_tests.count;
	return t;
}

void
finished_adding_pending_tests(void)
{
	size_t count = pending_tests.count;
	running_tests.array = malloc(count * sizeof *running_tests.array);
	running_tests.front = running_tests.array;
}

void
test_is_running(struct test *t)
{
	running_tests.array[running_tests.count++] = t;
}

void
all_tests_are_running(void)
{
	size_t count = running_tests.count;
	completed_tests.array = malloc(count * sizeof *completed_tests.array);
	completed_tests.front = completed_tests.array;
}

void
test_completed(pid_t pid, int status)
{
	struct test **t;
	assert(running_tests.count > 0);
	for (t = running_tests.front; (*t)->pid != pid; ++t) {
		;
	}

	(*t)->status = status;

	fprintf(stderr, "debug: test %s terminated, wait status: %u, exited normally: %u, exit status %u\n", (*t)->path, (*t)->status, WIFEXITED((*t)->status), WEXITSTATUS((*t)->status));

	/*
	Optimization: advance front of array if first element must be removed. Saves having to memmove rest of array.
	It also matches the average case scenario where most tests run so quickly, they will terminate in the same order they were started.
	*/
	--running_tests.count;
	if (t == running_tests.front) {
		++running_tests.front;
	} else {
		size_t block_size = (running_tests.count * sizeof *running_tests.front) - (t - running_tests.front);
		memmove(t, t+1, block_size);
	}

	completed_tests.array[completed_tests.count++] = *t;
}
