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
#include <stdlib.h> // sugo.h, malloc,
#include <stdio.h> // sugo.h
#include "sugo.h"
#include <string.h> // strcmp,

int
main(void)
{
	// verify repeadatly push and pop
	init_tests_module();
	for (int i = 1; i < 100; ++i) {
		char *path = malloc(2);
		path[0] = i;
		path[1] = '\0';
		add_pending_test(path);
	}
	for (int i = 1; i < 100; ++i) {
		TEST(i, next_pending_test()->path[0] == i);
	}
	return 0;
}
