/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2025 Pavel Nadein
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
 * Singleton implementation for C++.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>

#ifndef __cplusplus
#error "This project requires C++ compilation."
#endif

#if __cplusplus < 201103L
#error "This project requires C++11 or later."
#endif

/**
 * SingletonBase is a template class that provides a thread-safe singleton implementation.
 * It uses a static local variable to ensure that the singleton instance is created only once, even in the presence of multiple threads.
 * The class is designed to be inherited by the actual singleton class, which must provide a constructor that accepts a Token struct.
 * The getInstance() method returns a shared pointer to the singleton instance, allowing for safe access and automatic cleanup when the instance is no longer needed.
 *
 * Example usage:
 * class MySingleton : public SingletonBase<MySingleton> {
 * public:
 *     MySingleton(Token) {
 *         // Initialization code here
 *     }
 * };
 * auto instance = MySingleton::getInstance();
 *
 * @tparam T The type of the singleton class that inherits from SingletonBase.
 */
template <typename T>
class SingletonBase
{
public:
	SingletonBase(const SingletonBase &) = delete;
	SingletonBase &operator=(const SingletonBase &) = delete;

	static std::shared_ptr<T> getInstance()
	{
		static std::shared_ptr<T> instance = std::make_shared<T>(Token{});
		return instance;
	}

protected:
	struct Token { };
	SingletonBase() = default;
};

#endif /* SINGLETON_H */
