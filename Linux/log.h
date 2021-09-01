#ifndef __LOG_H__
#define __LOG_H__

void get_timestamp(char *dest, size_t size);
void log_to_file(const char *filename, const char *format, ...);

#endif /* __LOG_H__*/
