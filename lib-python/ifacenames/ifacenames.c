/*
    ifacenames - get logical interface names from /etc/network/interfaces
    Copyright (C) 2008  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _GNU_SOURCE

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>


/* PROTOTYPES */

static void fatal(const char* fmt, ...)
__attribute__ ((noreturn, format(printf, 1, 2)));


/* GLOBALS */

const char *program_name = "ifacenames";

const char * const scm_date = "$Date$";

const char * const scm_revision = "$Revision$";


/* FUNCTIONS */

static void help(void)
{
	printf(
"Usage: %s <options>\n"
"\n"
"Options:\n"
"\t-h, --help\t\tthis help\n"
"\t-V, --version\t\tcopyright and version information\n"
"\t--allow CLASS\t\tignore non-\"allow-CLASS\" interfaces\n"
"\t-i, --interfaces FILE\tuse FILE for interface definitions\n",
	       program_name);
}

static void version(void)
{
	const char *spc = strchr(scm_revision, ' ');
	int rev = spc != NULL ? atoi(spc + 1) : -1;

	printf(
"%s revision %d\n"
"Copyright (C) 2008  Proformatique\n"
"\n"
"This program is free software: you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation, either version 3 of the License, or\n"
"(at your option) any later version.\n",
	       program_name, rev);
}

static void fatal(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "%s: ", program_name);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	abort();
}

/* for tests : */
#define INITIAL_LINE_BUFFER_SIZE	4
#define MINIMAL_READ_BUFFER_SIZE	2

/* for production : */
/* #define INITIAL_LINE_BUFFER_SIZE	4096 */
/* #define MINIMAL_READ_BUFFER_SIZE	16 */

struct line {
	char *buf;
	size_t len;	/* excluding the trailing \0 */
	size_t bufsz;	/* whole buffer size, including any trailing \0 */
};

static struct line line_new(size_t bufsz)
{
	struct line line;

	assert(((ssize_t)bufsz) > 0);

	line.buf = calloc(bufsz, 1);
	line.len = 0;
	line.bufsz = bufsz;

	if (line.buf == NULL)
		fatal("line_new(): memory allocation failed\n");

	return line;
}

#define STANDARD_LINE_ASSERTIONS					\
	assert(line->buf != NULL);					\
	assert(((ssize_t)line->bufsz) > 0);				\
	assert(line->len < line->bufsz);

static void line_realloc(struct line *line, size_t bufsz)
{
	STANDARD_LINE_ASSERTIONS
	assert(((ssize_t)bufsz) > 0);
	assert(line->len < bufsz);

	line->buf = realloc(line->buf, bufsz);
	line->bufsz = bufsz;

	if (line->buf == NULL)
		fatal("line_realloc(): memory allocation failed\n");
}

static int line_is_empty(const struct line *line)
{
	STANDARD_LINE_ASSERTIONS
	
	return 0 == line->len;
}

static void line_clear(struct line *line)
{
	STANDARD_LINE_ASSERTIONS

	line->buf[0] = '\0';
	line->len = 0;
}

static void line_shift_left(struct line *line, size_t shift_nb)
{
	STANDARD_LINE_ASSERTIONS
	assert(((ssize_t)shift_nb) >= 0);

	if (shift_nb == 0)
		return;

	if (shift_nb < line->len) {
		memmove(line->buf, line->buf + shift_nb,
			line->len + 1 - shift_nb);
		line->len -= shift_nb;
	} else {
		line->buf[0] = '\0';
		line->len = 0;
	}
}

static void line_trunc(struct line *line, size_t newlen)
{
	STANDARD_LINE_ASSERTIONS
	assert(((ssize_t)newlen) >= 0);
	assert(newlen <= line->len);

	line->buf[newlen] = '\0';
	line->len = newlen;
}

static int line_endswith(const struct line *line, const char *s)
{
	size_t len = strlen(s);

	STANDARD_LINE_ASSERTIONS

	if (line->len < len)
		return 0;

	return 0 == strcmp(&line->buf[line->len - len], s);
}

#if 0
static int line_startswith(const struct line *line, const char *s)
{
	size_t len = strlen(s);

	STANDARD_LINE_ASSERTIONS

	if (line->len < len)
		return 0;

	return 0 == strncmp(line->buf, s, len);
}
#endif

static void line_rstrip(struct line *line)
{
	STANDARD_LINE_ASSERTIONS

	for (ssize_t pos = ((ssize_t)line->len) - 1; pos >= 0; pos--) {
		if (!isspace(line->buf[pos]))
			return;
		line->buf[pos] = '\0';
		line->len--;
	}
}

static void line_lstrip(struct line *line)
{
	size_t initial_spaces;

	STANDARD_LINE_ASSERTIONS

	for (initial_spaces = 0; initial_spaces < line->len; initial_spaces++)
		if (!isspace(line->buf[initial_spaces]))
			break;

	line_shift_left(line, initial_spaces);
}

static void line_strip(struct line *line)
{
	line_rstrip(line);
	line_lstrip(line);
}

static void line_remove_trailing_newline(struct line *line)
{
	STANDARD_LINE_ASSERTIONS

	if (line_endswith(line, "\n"))
		line_trunc(line, line->len - 1);
}

static void line_free(struct line *line)
{
	STANDARD_LINE_ASSERTIONS

	free(line->buf);
	line->buf = NULL;
	line->bufsz = 0;
	line->len = 0;
}

/* Really read a complete line, reallocating if needed, and aborting on errors.
   Return 0 on "end of file", else 1 */
static int line_fgets_append(struct line *line, FILE* f)
{
	size_t remaining_bufsz;
	char *s;

	STANDARD_LINE_ASSERTIONS

	do {
		remaining_bufsz = line->bufsz - line->len;
		if (remaining_bufsz < MINIMAL_READ_BUFFER_SIZE) {
			/* NOTE: not a while, voluntarily */
			line_realloc(line, line->bufsz * 2);
			remaining_bufsz = line->bufsz - line->len;
		}

		s = fgets(line->buf + line->len, remaining_bufsz, f);
		if (s == NULL && ferror(f)) {
			perror(program_name);
			fatal("read_interfaces_line(): aborting due to "
			      "previous error\n");
		}

		/* the standard libc should really be dumped in a trash... */
		line->len += strlen(&line->buf[line->len]);

		if (s == NULL) /* end of file */
			return 0;

		assert(line->len > 0);

	} while (line->buf[line->len - 1] != '\n');

	/* not end of file */
	return 1;
}

static int read_interfaces_line(FILE* f, struct line *line)
{
	int f_run;

retry:
	do {
		line_clear(line);
		f_run = line_fgets_append(line, f);
		line_lstrip(line);
	} while ((line->len == 0 || line->buf[0] == '#') && f_run);

	if (line->len == 0 || line->buf[0] == '#')
		return 0;

	line_remove_trailing_newline(line);

	while (line_endswith(line, "\\")) {
		line_trunc(line, line->len - 1);
		if (!f_run) {
			fprintf(stderr,
				"%s: read_interfaces_line(): WARNING "
				"continued line at end of file\n",
				program_name);
			break;
		}
		f_run = line_fgets_append(line, f);
		line_remove_trailing_newline(line);
	}

	line_strip(line);

	if (line_is_empty(line))
		goto retry;

	return 1;
}

int main(int argc, char *argv[])
{
	struct line line;
	static const char* allow = "";
	static const char* interfaces = "/etc/network/interfaces";
	FILE *f;

	if (argv[0])
		program_name = argv[0];

	while (1) {
		#define ALLOW 257
		static struct option long_options[] = {
			/* name        has_arg *flg val */
			{"allow",      1,      0,   ALLOW},
			{"help",       0,      0,   'h'},
			{"interfaces", 1,      0,   'i'},
			{"version",    0,      0,   'V'},
			{0, 0, 0, 0}
		};
		int opt;

		opt = getopt_long(argc, argv, "hi:V", long_options, NULL);

		if (opt == -1)
			break;

		switch (opt) {
		case ALLOW:
			allow = optarg;
			break;

		case 'h':
			help();
			exit(0);

		case 'i':
			interfaces = optarg;
			break;

		case 'V':
			version();
			exit(0);

		case '?':
			goto bad_cmd_line;

		default:
			fatal("internal error while parsing command line\n");
		}
	}

	if (optind != argc) {
bad_cmd_line:
		fprintf(stderr, "%s: Use --help for help\n",
			program_name);
		exit(42);
	}

	f = fopen(interfaces, "r");
	if (f == NULL)
		fatal("couldn't read interfaces file \"%s\"\n", interfaces);

	for (line = line_new(INITIAL_LINE_BUFFER_SIZE);
	     read_interfaces_line(f, &line); ) {
		/* TODO */
		/* <TEST> */
		puts(line.buf);
		/* </TEST> */
	}
	line_free(&line);

	fclose(f);

	return 0;
}
