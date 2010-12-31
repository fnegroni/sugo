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

#include <argp.h> // options.h, argp*,
#include "options.h"
#include <stdlib.h> // abort, exit,
#include "test.h" // tests.h,
#include "tests.h" // add_pending_test, pending_tests,

unsigned int verbosity_level = 1;

const char *argp_program_version = "0.1";
const char *argp_program_bug_address = "<f.e.negroni@gmail.com>";
error_t argp_err_exit_status;
static const char *const NONPOSITIONAL_ARGS_DOC = "TEST...";
static const char *const PROGRAM_DOC = "sugo -- a test framework for system routines.";

static error_t
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

