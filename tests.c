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

struct tests_queue pending_tests;
struct tests_vector running_tests;

void
init_tests_module(void)
{
	pending_tests.count = 0;
	pending_tests.head.test = 0;
	pending_tests.tail.test = 0;
	pending_tests.head.next = tail;
	pending_tests.tail.next = tail;
	pending_tests.front = &pending_tests.tail;
	pending_tests.back = &pending_tests.head;
	running_tests.count = 0;
	running_tests.array = 0;
}

void
add_pending_test(const char *path)
{
	pending_tests.back = pending_tests.back->next = malloc(sizeof pending_tests.head);
	pending_tests.back->next = tail;
	pending_tests.back->test = new_test(path);
	++tests.count;
}

struct test *
pop_pending_test(void)
{
	struct test *t = front->next->test;
	front = front->next;
	return t;
}

