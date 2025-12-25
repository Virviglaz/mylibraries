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
 * Digital filters.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __FILTERS_H__
#define __FILTERS_H__

#ifndef __cplusplus
#error This header is for C++ only.
#endif /* __cplusplus */

#include <cstddef>
#include <algorithm>
#include <array>
#include <cmath>

/**
 * Simple Kalman filter implementation.
 *
 * Template parameter T should be a floating point type.
 */
template <typename T = float>
class KalmanFilter
{
public:
	/**
	 * Constructor to initialize the Kalman filter with process noise,
	 * measurement noise, and initial estimation error.
	 *
	 * @param a_ProcessNoise      Process noise covariance.
	 * @param a_MeasurementNoise  Measurement noise covariance.
	 * @param a_EstimationError   Initial estimation error covariance.
	 */
	KalmanFilter(T a_ProcessNoise, T a_MeasurementNoise, T a_EstimationError)
		: processNoise(a_ProcessNoise),
		  measurementNoise(a_MeasurementNoise),
		  estimationError(a_EstimationError),
		  lastEstimate(0) {}

	T update(T measurement) noexcept
	{
		// Prediction update
		estimationError += processNoise;

		// Measurement update
		T kalmanGain = estimationError / (estimationError + measurementNoise);
		lastEstimate = lastEstimate + kalmanGain * (measurement - lastEstimate);
		estimationError = (1 - kalmanGain) * estimationError;

		return lastEstimate;
	}

private:
	T processNoise;
	T measurementNoise;
	T estimationError;
	T lastEstimate;
};

/**
 * Simple moving average filter implementation.
 *
 * Template parameter T should be a floating point type.
 */
template <typename T = float, int windowSize = 5>
class MovingAverageFilter
{
public:
	/** Constructor to initialize the moving average filter. */
	MovingAverageFilter() : sum(0), count(0), index(0) {}

	/**
	 * Update the filter with a new value and get the current average.
	 *
	 * @param newValue  New value to add to the filter.
	 * @return          Current average after adding the new value.
	*/
	T update(T newValue) noexcept
	{
		sum -= buffer[index];
		buffer[index] = newValue;
		sum += newValue;

		index = (index + 1) % windowSize;
		if (count < windowSize) {
			++count;
		}

		return sum / static_cast<T>(count);
	}

private:
	T buffer[windowSize] = {0};
	T sum;
	size_t count;
	size_t index;
};

/**
 * Exponential Moving Average (EMA)
 *
 * Template parameter T should be a floating point type.
 */
template <typename T = float>
class ExponentialMovingAverage
{
public:
	/**
	 * Constructor to initialize the EMA with a given alpha.
	 *
	 * @param alpha Smoothing factor between 0 and 1 (default is 0.1).
	 * 			 Higher alpha discounts older observations faster.
	 */
    explicit ExponentialMovingAverage(T alpha = static_cast<T>(0.1))
        : alpha_(alpha), initialized_(false), last_(0) {}

    T update(T value) noexcept
    {
        if (!initialized_) {
            last_ = value;
            initialized_ = true;
            return last_;
        }
        last_ = alpha_ * value + (static_cast<T>(1) - alpha_) * last_;
        return last_;
    }

    void reset(T v = 0) noexcept { last_ = v; initialized_ = false; }

private:
    T alpha_;
    bool initialized_;
    T last_;
};

/**
 * Median filter with fixed small window
 *
 * Template parameter T should be a floating point type.
 * windowSize defines the size of the moving window.
 */
template <typename T = float, int windowSize = 5>
class MedianFilter
{
    static_assert(windowSize > 0, "windowSize must be positive");
public:
	/**
	 * Constructor to initialize the median filter.
	 *
	 * Initializes the buffer to zeros.
	 */
    MedianFilter() : index_(0), count_(0) { buffer_.fill(static_cast<T>(0)); }

	/**
	 * Update the filter with a new value and get the current median.
	 *
	 * @param value  New value to add to the filter.
	 * @return       Current median after adding the new value.
	 */
    T update(T value) noexcept
    {
        buffer_[index_] = value;
        index_ = (index_ + 1) % windowSize;
        if (count_ < windowSize) ++count_;

        // copy to temp and compute median
        std::array<T, windowSize> tmp;
        for (int i = 0; i < static_cast<int>(count_); ++i)
            tmp[i] = buffer_[i];
        auto mid = tmp.begin() + (count_ / 2);
        std::nth_element(tmp.begin(), mid, tmp.begin() + count_);
        if (count_ % 2 == 1)
            return *mid;
        // even count: take average of two middle elements
        auto max_of_lower = *std::max_element(tmp.begin(), mid);
        return (max_of_lower + *mid) / static_cast<T>(2);
    }

	/**
	 * Reset the filter state.
	 *
	 * @param v  Value to initialize the buffer with (default is 0).
	 */
    void reset(T v = 0) noexcept { buffer_.fill(v); index_ = 0; count_ = 0; }

private:
    std::array<T, windowSize> buffer_;
    int index_;
    int count_;
};

/**
 * First-order low-pass filter (continuous -> discrete approximation)
 *
 * Template parameter T should be a floating point type.
 */ 
template <typename T = float>
class FirstOrderLowPass
{
public:
    /**
	 * Cutoff in Hz, dt in seconds (if dt changes per sample, call update(sample, dt))
	 *
	 * @param cutoff_hz  Cutoff frequency in Hz (default is 1.0 Hz).
	 * @param initial    Initial value of the filter (default is 0).
	 */
    FirstOrderLowPass(T cutoff_hz = static_cast<T>(1.0), T initial = static_cast<T>(0))
        : cutoff_hz_(cutoff_hz), last_(initial), initialized_(false) {}

	/**
	 * Update the filter with a new sample and time delta.
	 * @param sample  New sample to add to the filter.
	 * @param dt      Time delta in seconds since the last update.
	 * @return        Filtered value after adding the new sample.
	 */
    T update(T sample, T dt) noexcept
    {
        if (dt <= static_cast<T>(0)) return last_;
        const T pi = static_cast<T>(3.14159265358979323846);
        const T tau = static_cast<T>(1) / (static_cast<T>(2) * pi * cutoff_hz_);
        const T alpha = dt / (tau + dt);
        if (!initialized_) { last_ = sample; initialized_ = true; return last_; }
        last_ = last_ + alpha * (sample - last_);
        return last_;
    }

	/**
	 * Reset the filter state.
	 *
	 * @param v  Value to initialize the filter with (default is 0).
	 */
    void reset(T v = 0) noexcept { last_ = v; initialized_ = false; }

private:
    T cutoff_hz_;
    T last_;
    bool initialized_;
};

/**
 * First-order high-pass filter (continuous -> discrete approximation)
 *
 * Template parameter T should be a floating point type.
 */
template <typename T = float>
class FirstOrderHighPass
{
public:
	/**
	 * Cutoff in Hz, dt in seconds (if dt changes per sample, call update(sample, dt))
	 *
	 * @param cutoff_hz  Cutoff frequency in Hz (default is 1.0 Hz).
	 * @param initial    Initial value of the filter (default is 0).
	 */
    FirstOrderHighPass(T cutoff_hz = static_cast<T>(1.0), T initial = static_cast<T>(0))
        : cutoff_hz_(cutoff_hz), last_y_(initial), last_x_(initial), initialized_(false) {}

	/**
	 * Update the filter with a new sample and time delta.
	 *
	 * @param sample  New sample to add to the filter.
	 * @param dt      Time delta in seconds since the last update.
	 * @return        Filtered value after adding the new sample.
	 */
    T update(T sample, T dt) noexcept
    {
        if (dt <= static_cast<T>(0)) return last_y_;
        const T pi = static_cast<T>(3.14159265358979323846);
        const T tau = static_cast<T>(1) / (static_cast<T>(2) * pi * cutoff_hz_);
        const T alpha = tau / (tau + dt);
        if (!initialized_) { last_x_ = sample; last_y_ = static_cast<T>(0); initialized_ = true; }
        T y = alpha * (last_y_ + sample - last_x_);
        last_x_ = sample;
        last_y_ = y;
        return y;
    }

	/**
	 * Reset the filter state.
	 *
	 * @param v  Value to initialize the filter with (default is 0).
	 */
    void reset(T v = 0) noexcept { last_x_ = last_y_ = v; initialized_ = false; }

private:
    T cutoff_hz_;
    T last_y_;
    T last_x_;
    bool initialized_;
};

#endif // __FILTERS_H__
