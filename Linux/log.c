#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

#include "log.h"

void get_timestamp(char *dest, size_t size)
{
	time_t t = time(NULL);
	struct tm* to = localtime(&t);

	strftime(dest, size, "%d %b %Y %T", to);
}

static pthread_mutex_t lock;
void log_to_file(const char *filename, const char *format, ...)
{
	va_list arg;
	int fd;
	char buf[1024];
	size_t size;

	pthread_mutex_lock(&lock);

	fd = open(filename, O_CREAT | O_WRONLY, S_IRUSR);
	if (fd <= 0) {
		int err = errno;
		fprintf(stderr, "Write file %s error: %s\n",
			filename, strerror(err));
		goto exit;
	}

	get_timestamp(buf, sizeof(buf));
	size = strlen(buf);

	va_start(arg, format);
	vsnprintf(buf + size, sizeof(buf) - size - 1,  format, arg);
	va_end(arg);

	close(fd);
exit:
	pthread_mutex_unlock(&lock);
}
