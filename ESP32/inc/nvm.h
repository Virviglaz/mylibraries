#ifndef __NVM_H__
#define __NVM_H__

#include <stdint.h>
#include <stddef.h>

void nvm_read(void *dst, size_t size);
void nvm_write(void *src, size_t size);

#endif /* __NVM_H__ */
