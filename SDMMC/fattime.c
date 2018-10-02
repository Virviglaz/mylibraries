/* Martin Thomas 4/2009 */

#include "fattime.h"
#include "rtc.h"

uint32_t get_fattime (void)
{
	#if USE_FAKE_TIME == 0
		RTC_t rtc;
		rtc_gettime( &rtc );
		
		return  ((rtc.year - 1980) << 25)
				| (rtc.month << 21)
				| (rtc.mday << 16)
				| (rtc.hour << 11)
				| (rtc.min << 5)
				| (rtc.sec >> 1);
	#else
		return ((2018 - 1980) << 25)
				| (1 << 21)
				| (1 << 16)
				| (12 << 11)
				| (0 << 5)
				| (0 >> 1);
	#endif
}

