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
	pending_tests.ph.test = 0;
	pending_tests.ph.next = &pending_tests.ph;
	pending_tests.front = pending_tests.back = &pending_tests.ph.next;
	running_tests.count = 0;
	running_tests.array = 0;
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
pop_pending_test(void)
{
	struct test *t = (*pending_tests.front)->test;
	pending_tests.front = &(*pending_tests.front)->next;
	return t;
}

void
finished_adding_pending_tests(void)
{
	// init running tests as array? or tree?
	// what is the best data structure to have tess and look them up?
}

void
insert_running_test(void)
{
}
