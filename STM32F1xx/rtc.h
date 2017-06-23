#ifndef RTC_H_
#include "stm32f10x_rtc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x.h"

typedef struct {
	u16 year;	/* 1..4095 */
	uint8_t  month;	/* 1..12 */
	uint8_t  mday;	/* 1.. 31 */
	uint8_t  wday;	/* 0..6, Sunday = 0*/
	uint8_t  hour;	/* 0..23 */
	uint8_t  min;	/* 0..59 */
	uint8_t  sec;	/* 0..59 */
	uint8_t  dst;	/* 0 Winter, !=0 Summer */
	u16 TimeLogSec;
	volatile u8 SecFlag;
} RTC_t;

int rtc_init(void);
void rtc_gettime (RTC_t*);					/* Get time */
void rtc_settime (const RTC_t*);				/* Set time */


#endif
