[spec]
location = ; the default is basename of this file name without the extension: relative to current path
name = a-1 ; the default name is to remove the .spec extension if found. An error is printed otherwise

[descriptors]
; all except descriptor 0, that's standard input
	1 = a-1.out ; this is the default
	2 = a-1.err ; this is the default
	3 = a-1.fd3 ; this is another default
; to open a file descriptor with a default name, just list it empty
	4 = 
; if two fd's point to the same file, they are dup'ed.
; use % to mean the name of this test, which is the name of this spec file without the spec extension, unless a name attribute is specified
	5 = %.fd4

[input]
	filename = path-relative-to-location-of-this-spec
	text = arbitrary text in latin1
	from-test = test-name:descriptor ;auto-dependency, descriptor must be found in the other test's spec, or an error will be printed. This can be used to generate random inputs since a test can be and do anything. Even a shell script.
	concurrent = false ; the default. A value of true allows the two tests to run concurrently as if in a pipe.

[pre]
; list all tests which this test depends on: if those are not successful, do not run.
; All specs which list a pre are not scheduled until a pre is found. Testing can start anyway: most tests don't have a pre, only the ones which are expensive to run do, or the ones that use the output of another test.
	tests = a-0

