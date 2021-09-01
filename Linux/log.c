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
	char timestamp[32];

	pthread_mutex_lock(&lock);

	FILE *file = fopen(filename, "aw");
	if (!file) {
		int err = errno;
		fprintf(stderr, "Write file %s error: %s\n",
			filename, strerror(err));
		pthread_mutex_unlock(&lock);
		return;
	}

	get_timestamp(timestamp, sizeof(timestamp));
	strcpy(timestamp + strlen(timestamp), ": ");

	fwrite(timestamp, 1, strlen(timestamp), file);

	va_start(arg, format);
	vfprintf(file, format, arg);
	va_end(arg);

	fclose(file);

	pthread_mutex_unlock(&lock);
}
