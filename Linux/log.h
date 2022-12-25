#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

void get_timestamp(char *dest, size_t size);
void log_to_file(const char *filename, const char *format, ...)
#ifdef __GNUC__
          __attribute__ (( format( printf, 2, 3 ) ))
#endif
;

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__*/
