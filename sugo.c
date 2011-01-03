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

#include <stdio.h> // *printf,
#include <stdlib.h> // abort,
#include <string.h> // basename (GNU), strerror,
#include <unistd.h> // fork, pid_t, close, dup2,
#include <sys/types.h> // wait, open
#include <sys/wait.h> // wait,
#include "test.h" // tests.h, struct test,
#include "tests.h" // tests, init_tests_module, running_tests, all_tests_are_running, finished_adding_pending_tests, test_completed, test_is_running, next_pending_test,
#include <sys/stat.h> // open,
#include <fcntl.h> // open,
#include "options.h" // verbosity_level,
#include <errno.h> // errno,

static void
redirect_std_fds(const char *testpath)
{
	// TODO: any errors printed by the child process go to fd 3.
	// fd 3 is a dup() of Sugo's main process fd 2.
	// This should also be set close on exec to not get it inherited by the child process
	// Any errors before the exec will be sent there.
	// In order to ensure Sugo's stderr is shared correctly, we use a mutex: any writing to stderr is therefore serialized and synchronized.
	// Could be implemented using lockf() or fcntl...
	// Are locks inherited over forks? if not, problem? No, it's exactly what we want: Sugo's main process gets the lock, and the children have to acquire it. They can wait.
	// Normal output from Sugo goes to stdout anyway, stderr is reserved for error conditions, which should be very few and far in between.
	// FIXME: does close on exec take place if the image cannot be found? to test...

	if (close(1) || close(2)) {
		print_on_sugo_stderr();
		//fprintf(stderr, "Error closing standard file descriptors 0, 1 and 2: %s\n", strerror(errno));
		abort();
	}
	size_t testpathlen = strlen(testpath);
	char path[testpathlen+4+1];
	char *pathext = path+testpathlen;
	strcpy(path, testpath);
	strcpy(pathext, ".in");
	open(path, O_RDONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	strcpy(pathext, ".out");
	open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	strcpy(pathext, ".err");
	open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
}

static void
spawn_tests(void)
{
	struct test *t;

	while ((t = next_pending_test())) {
		if (verbosity_level) fprintf(stderr, "Spawning test %s: ", t->path);
		pid_t pid = fork();
		if (pid == 0) {
			redirect_std_fds(t->path);
			execl(t->path, basename(t->path), (void *)0);
			fprintf(stderr, "error: failed to exec %s\n", t->path);
			abort();
		} else if (-1 == pid) {
			fprintf(stderr, "error: unable to fork new test.\n");
			return;
		} else {
			t->pid = pid;
			test_is_running(t);
			fprintf(stderr, "pid %d\n", t->pid);
		}
	}
	all_tests_are_running();
}

static void
wait_for_tests(void)
{
	while (running_tests.count) {
		int status;
		pid_t pid = wait(&status);
		test_completed(pid, status);
	}
}

static void
process_tests(void)
{
	spawn_tests();
	wait_for_tests();
	//print_stats();
}

static void
setup_file_descriptors(void)
{
	if (-1 == dup2(2, 3)) {
		fprintf(stderr, "Error dup'ing file descriptor 2 (stderr) into 3: %s\n", strerror(errno));
		abort();
	}
	if (-1 == fcntl(3, F_SETFD, FD_CLOEXEC)) {
		fprintf(stderr, "Error setting close on exec flag on file descriptor 3: %s\n", strerror(errno));
		abort();
	}
	if (close(0)) {
		fprintf(stderr, "Error closing file descriptor 0 (stdin): %s\n", strerror(errno));
		abort();
	}
}

int
main(int argc, char **argv)
{
	setup_file_descriptors();
	init_tests_module();
	parse_args(argc, argv);
	finished_adding_pending_tests();
	process_tests();

	return 0;
}
