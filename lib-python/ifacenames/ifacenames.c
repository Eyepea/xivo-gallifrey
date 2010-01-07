/*
    ifacenames - get logical interface names from /etc/network/interfaces
    Copyright (C) 2008-2010  Proformatique

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
#include <limits.h>


/* CONSTANTS */

#define DEFAULT_INTERFACES_FILEPATH "/etc/network/interfaces"

/* for tests : */
/* #define INITIAL_LINE_BUFFER_SIZE	4 */
/* #define MINIMAL_READ_BUFFER_SIZE	2 */

/* for production : */
#define INITIAL_LINE_BUFFER_SIZE	4096
#define MINIMAL_READ_BUFFER_SIZE	16


/* PROTOTYPES */

static void fatal(const char* fmt, ...)
__attribute__ ((noreturn, format(printf, 1, 2)));

#define FATAL_MEM()							\
	fatal("%s(): memory allocation failed\n", __FUNCTION__)


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
"Copyright (C) 2008-2010  Proformatique\n"
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
		FATAL_MEM();

	return line;
}

static struct line line_new_filled(const char *str, size_t len)
{
	struct line line;

	assert(((ssize_t)len) >= 0);

	line.buf = malloc(len + 1);
	line.len = len;
	line.bufsz = len + 1;

	if (line.buf == NULL)
		FATAL_MEM();

	memcpy(line.buf, str, len);
	line.buf[len] = '\0';

	return line;
}

#define STANDARD_LINE_ASSERTIONS					\
	assert(line != NULL);						\
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
		FATAL_MEM();
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

/* Copy the first word of line starting after potential spaces at position 
   *pos to out_buf which is a buffer of size out_bufsz.  Update *pos so that
   it points to the first space in line after this word or to the end of the
   line if the word is not followed by spaces.  out_buf is always zero
   terminated and this function returns its length not including the
   terminating '\0'.
   Note that if the line contains only spaces or is empty, *pos is updated to
   point to the terminating '\0' of line, out_buf[0] is set to '\0' and 0 is
   returned. */
static size_t line_extract_word(const struct line *line, size_t *pos,
                                char *out_buf, size_t out_bufsz)
{
	size_t out_pos = 0;

	STANDARD_LINE_ASSERTIONS
	assert(pos != NULL);
	assert(*pos <= line->len);
	assert(out_buf != NULL);
	assert(((ssize_t)out_bufsz) > 0);

	/* skip leading spaces */
	while (line->buf[*pos] && isspace(line->buf[*pos]))
		(*pos)++;

	/* copy the first word to out_buf and make *pos
	   point on the first space after it in line */
	while (line->buf[*pos] && !isspace(line->buf[*pos])) {
		if (out_pos < out_bufsz - 1) {
			out_buf[out_pos] = line->buf[*pos];
			out_pos++;
		}
		(*pos)++;
	}

	out_buf[out_pos] = '\0';

	return out_pos;
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
			if (line->bufsz >= INT_MAX / 2)
				fatal("fatal: line well too long detected");
			line_realloc(line, line->bufsz * 2);
			remaining_bufsz = line->bufsz - line->len;
		}

		s = fgets(line->buf + line->len, (int)remaining_bufsz, f);
		if (s == NULL && ferror(f)) {
			perror(program_name);
			fatal("%s(): aborting due to "
			      "previous error\n", __FUNCTION__);
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
				"%s: %s(): WARNING "
				"continued line at end of file\n",
				program_name, __FUNCTION__);
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

struct linked_str {
	struct line line;
	struct linked_str *next;
};

static struct linked_str *add_linked_str(struct linked_str *next,
                                         const char *str,
					 size_t len)
{
	struct linked_str *new_link;

	new_link = malloc(sizeof(*new_link));

	if (new_link == NULL)
		FATAL_MEM();

	new_link->line = line_new_filled(str, len);
	new_link->next = next;

	return new_link;
}

static int is_in_linked_str(struct linked_str *first, const char *str)
{
	for (struct linked_str *current = first;
	     current != NULL;
	     current = current->next) {
		if (0 == strcmp(current->line.buf, str))
			return 1;
	}

	return 0;
}

static void linked_str_free(struct linked_str *first)
{
	struct linked_str *current = first;
	struct linked_str *next;

	while (current != NULL) {
		next = current->next;
		line_free(&current->line);
		free(current);
		current = next;
	}
}

int main(int argc, char *argv[])
{
	struct line line;
	static const char *allow = NULL;
	static const char *interfaces = DEFAULT_INTERFACES_FILEPATH;

	#define WORD_BUFFER_SIZE 80
	static char firstword[WORD_BUFFER_SIZE];
	static char iface_name[WORD_BUFFER_SIZE];

	struct linked_str *allowed_ifaces = NULL;

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

	line = line_new(INITIAL_LINE_BUFFER_SIZE);

	f = fopen(interfaces, "r");
	if (f == NULL)
		fatal("couldn't read interfaces file \"%s\"\n", interfaces);

	if (allow != NULL) {
		/* load allowed interfaces if the --allow opt has been given */
		while (read_interfaces_line(f, &line)) {
			const char *allowed;
			size_t pos = 0;
			size_t wordlen;

			line_extract_word(&line, &pos, firstword,
					  WORD_BUFFER_SIZE);

			if (strcmp(firstword, "auto") == 0)
				allowed = firstword;
			else if (strncmp(firstword, "allow-", 6) == 0)
				allowed = firstword + 6;
			else
				continue;

			if (strcmp(allow, allowed) != 0)
				continue;

			while ((wordlen = line_extract_word(&line, &pos, 
							    iface_name,
							    WORD_BUFFER_SIZE)))
				allowed_ifaces = add_linked_str(allowed_ifaces,
								iface_name,
								wordlen);
		}
		rewind(f);
	}

	/* last read pass and emit output */
	while (read_interfaces_line(f, &line)) {
		size_t pos = 0;
		size_t wordlen;

		line_extract_word(&line, &pos, firstword, WORD_BUFFER_SIZE);

		if (strcmp(firstword, "iface") == 0) {
			wordlen = line_extract_word(&line, &pos, iface_name,
						    WORD_BUFFER_SIZE);
			if (wordlen > 0) {
				if (allow == NULL
				    || is_in_linked_str(allowed_ifaces,
							iface_name))
					puts(iface_name);
			}
		}
	}

	/* cleanup */
	linked_str_free(allowed_ifaces);
	fclose(f);
	line_free(&line);

	return 0;
}
