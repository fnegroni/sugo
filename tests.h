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

/*
include
	test.h : struct test
*/

struct tests_queue_item {
	struct test *test;
	struct tests_queue_item *next;
};

struct tests_queue {
	size_t count;
	struct tests_queue_item ph;
	struct tests_queue_item **front, **back;
};

struct tests_vector {
	size_t count;
	struct test **array;
};

extern struct tests_queue pending_tests;
extern struct tests_vector running_tests;
extern struct tests_vector completed_tests;

extern void init_tests_module(void);
extern void add_pending_test(const char *path);
extern struct test *next_pending_test(void);
extern void all_tests_are_running(void);
extern void test_is_running(struct test *t);
extern void finished_adding_running_tests(void);
extern void test_completed(pid_t pid, int status);
