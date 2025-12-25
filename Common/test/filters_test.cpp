#include "filters.h"
#include <stdio.h>

int do_filters_test()
{
    KalmanFilter<float> kf(0.1f, 1.0f, 1.0f);
    MovingAverageFilter<float, 3> maf;

    // additional filters to test
    ExponentialMovingAverage<float> ema(0.2f);
    MedianFilter<float, 3> median;
    FirstOrderLowPass<float> lpf(1.0f);   // cutoff 1 Hz
    FirstOrderHighPass<float> hpf(0.5f);  // cutoff 0.5 Hz

    float measurements[] = {10.0f, 12.0f, 13.0f, 12.5f, 11.0f, 10.5f};
    const size_t num_measurements = sizeof(measurements) / sizeof(measurements[0]);

    // assume a fixed sample interval for the simple LPF/HPF test
    const float dt = 0.1f; // 100 ms

    for (size_t i = 0; i < num_measurements; ++i)
    {
        float sample = measurements[i];

        float kf_result = kf.update(sample);
        float maf_result = maf.update(sample);
        float ema_result = ema.update(sample);
        float median_result = median.update(sample);
        float lpf_result = lpf.update(sample, dt);
        float hpf_result = hpf.update(sample, dt);

        // Print the results for inspection
        printf("Sample: %.2f | Kalman: %.3f | MA3: %.3f | EMA: %.3f | Median3: %.3f | LPF: %.3f | HPF: %.3f\n",
               sample, kf_result, maf_result, ema_result, median_result, lpf_result, hpf_result);
    }

    // quick reset checks (sanity)
    ema.reset();
    median.reset(0.0f);
    lpf.reset(0.0f);
    hpf.reset(0.0f);

    // one more sample after reset
    float post = 9.0f;
    printf("After reset: EMA=%.3f Median=%.3f LPF=%.3f HPF=%.3f\n",
           ema.update(post), median.update(post), lpf.update(post, dt), hpf.update(post, dt));

    return 0;
}
