/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2026 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * C++ Linux Utilities Library
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <execinfo.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sys/resource.h>
#include <sys/times.h>
#include <cstdint>
#include <stdio.h>
#include <time.h>

#ifndef __cplusplus
#error This header requires a C++ compiler
#endif

#ifndef BT_BUF_SIZE
#define BT_BUF_SIZE 100
#endif
std::vector<std::string> get_backtrace()
{
	std::vector<std::string> result;
	void *addrlist[BT_BUF_SIZE];

	// retrieve current stack addresses
	int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void *));

	if (addrlen == 0)
	{
		result.push_back("<empty, possibly corrupt>");
		return result;
	}

	// resolve addresses into strings containing "filename(function+address)",
	// this array must be free()-ed
	char **symbollist = backtrace_symbols(addrlist, addrlen);

	for (int i = 0; i < addrlen; i++)
	{
		result.push_back(symbollist[i]);
	}

	free(symbollist);
	return result;
}

uint64_t get_memory_usage()
{
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	return static_cast<uint64_t>(usage.ru_maxrss) * 1024; // Convert from KB to Bytes
}

time_t get_cpu_time_us()
{
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
		return rusage.ru_utime.tv_sec * 1000000 +
			   rusage.ru_utime.tv_usec;
}

static time_t elapsed;
/**
 * @class BenchmarkTimer
 * @brief A simple RAII timer for benchmarking code execution time.
 */
class BenchmarkTimer
{
public:
	/** 
	 * @brief Constructs a BenchmarkTimer and starts timing.
	 *
	 * @param name Optional name for the timer to identify the benchmark.
	 * @param report If true, the elapsed time will be printed upon destruction.
	 */
	BenchmarkTimer(const char *name = nullptr, bool report = true)
		: name_(name), report_(report)
	{
		start_time_ = get_cpu_time_us();
	}

	~BenchmarkTimer()
	{
		time_t end_time = get_cpu_time_us();
		elapsed = end_time - start_time_;

		if (report_)
		{
			if (name_)
				printf("[BenchmarkTimer] %s: %ld us\n", name_, elapsed);
			else
				printf("[BenchmarkTimer] Elapsed time: %ld us\n", elapsed);
		}
	}

	/** 
	 * @brief Returns the elapsed time in microseconds.
	 *
	 * @return Elapsed time in microseconds.
	 */
	static time_t GetElapsedTime()
	{
		return elapsed;
	}
private:
	const char *name_;
	bool report_;
	time_t start_time_;
};

#endif // UTILS_H
