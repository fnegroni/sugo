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
#include "tests.h" // all declarations for this module
#include <stdlib.h> // malloc,

tests_list tests;

void
init_tests(void)
{
	tests.count = 0;
	tests.head.next = 0;
	tests.last = &tests.head;
}

void
push_test(const char *path)
{
	tests.last = tests.last->next = malloc(sizeof(struct tests_list_item));
	item->test = new_test(path);
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

