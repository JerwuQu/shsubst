#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SCAN_BUF_SZ 4096

// PARAM_BUF_SZ decides the maximum size of env var names and commands
#define PARAM_BUF_SZ 4096

static void printVar(const char *var)
{
	char *val = getenv(var);
	if (val)
		fputs(val, stdout);
}

static void execCmd(const char *cmd)
{
	fflush(stdout);
	system(cmd);
}

int main(int argc, char **argv)
{
	if (argc != 1) {
		fprintf(stderr, "%s\n\tReads from stdin, outputs to stdout.\n", argv[0]);
		return 1;
	}

	enum {
		S_NORMAL,
		S_DOLLAR,
		S_CAPTURE,
	} state = S_NORMAL;

	char captureEnd;
	void (*captureFn)(const char*);
	bool captureBackslashed = false;

	size_t bread;
	char scanBuf[SCAN_BUF_SZ];
	size_t paramI;
	char paramBuf[PARAM_BUF_SZ];
	while (1) {
		size_t offset = 0;
		if (!(bread = fread(scanBuf, 1, SCAN_BUF_SZ, stdin)))
			break;

		for (size_t i = 0; i < bread; i++) {
			switch (state) {
			case S_DOLLAR:
				if (scanBuf[i] == '$') {
					state = S_NORMAL;
				} else if (scanBuf[i] == '{') {
					state = S_CAPTURE;
					offset++;
					captureEnd = '}';
					captureFn = &printVar;
				} else if (scanBuf[i] == '(') {
					state = S_CAPTURE;
					offset++;
					captureEnd = ')';
					captureFn = &execCmd;
				} else {
					putc('$', stdout);
					state = S_NORMAL;
				}
				break;
			case S_NORMAL:
				if (scanBuf[i] == '$') {
					state = S_DOLLAR;
					fwrite(scanBuf + offset, 1, i - offset, stdout);
					offset = i + 1;
					paramI = 0;
				}
				break;
			case S_CAPTURE:
				if (captureBackslashed) {
					captureBackslashed = false;
				} else if (scanBuf[i] == '\\') {
					memcpy(paramBuf + paramI, scanBuf + offset, i - offset);
					paramI += i - offset;
					offset = i + 1;
					captureBackslashed = true;
				} else if (scanBuf[i] == captureEnd) {
					memcpy(paramBuf + paramI, scanBuf + offset, i - offset);
					paramI += i - offset;
					paramBuf[paramI] = 0;
					captureFn(paramBuf);
					state = S_NORMAL;
					offset = i + 1;
				}
				break;
			}
		}

		if (state == S_NORMAL) {
			fwrite(scanBuf + offset, 1, bread - offset, stdout);
		} else if (state == S_CAPTURE) {
			const size_t sz = bread - offset;
			if (paramI + sz >= PARAM_BUF_SZ) {
				fprintf(stderr, "fatal error: paramBuf overflow\n");
				return 1;
			}

			memcpy(paramBuf + paramI, scanBuf + offset, sz);
			paramI += sz;
		}
	}

	return 0;
}
