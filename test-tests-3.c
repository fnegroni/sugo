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

#include <unistd.h> // test.h
#include "test.h" // tests.h
#include "tests.h"
#include <stdlib.h> // sugo.h
#include <stdio.h> // sugo.h
#include "sugo.h"
#include <string.h> // strcmp,

int
main(void)
{
	// verify simple push pop from queue
	init_tests_module();
	add_pending_test("A");
	TEST(1, pending_tests.front == &pending_tests.ph.next)
	TEST(2, pending_tests.back == &pending_tests.ph.next->next)
	TEST(3, 0 == strcmp(next_pending_test()->path, "A"));
	TEST(4, pending_tests.front == &pending_tests.ph.next->next)
	TEST(5, pending_tests.back == &pending_tests.ph.next->next)
	return 0;
}
