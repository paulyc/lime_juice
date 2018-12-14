/**
 * Copyright (c) 2018 Paul Ciarlo <paul.ciarlo@gmail.com>
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
 */

#include <memory>
#include <iostream>
#include <atomic>
#include <csignal>
#include <cmath>
#include <cstring>

#include <unistd.h>

#include "lime_juice.h"

using namespace io::github::paulyc;

static lms_device_t *dev = nullptr;
static std::unique_ptr<lms_stream_t> stream;
static volatile bool running;

void cleanup() {
    if (stream != nullptr) {
        LimeLog::log(LimeLog::INFO, "Stopping Stream...");
        LMS::StopStream(stream.get());
        LimeLog::log(LimeLog::INFO, "Destroying Stream...");
        LMS::DestroyStream(dev, stream.get());
        stream.reset();
        running = false;
    }
    if (dev != nullptr) {
        LimeLog::log(LimeLog::INFO, "Destroying Lime...");
        LMS::Close(dev);
        dev = nullptr;
    }
}

static double normalizedGain = 0.5;

void mainLoop(bool autoGain) {
    lms_stream_status_t status;
    lms_stream_meta_t meta;
    size_t loopCount = 0;
    double maxMag = 0.0;
    constexpr size_t samples_per_recv = 0xFFFF;
    std::unique_ptr<float[]> buffer = std::unique_ptr<float[]>(new float[samples_per_recv * 2]);
    std::unique_ptr<int16_t[]> outBuffer = std::unique_ptr<int16_t[]>(new int16_t[samples_per_recv * 2]);
    LMS::StartStream(stream.get());
    running = true;
    std::signal(SIGINT, [](int sig) -> void { running = false; });
    while (running) {
        // (1 sample = I + Q)
        const int samples = LMS::RecvStream(stream.get(), buffer.get(), samples_per_recv, &meta, 5000);
        for (int si = 0; si < samples; ++si) {
            const double I = buffer[si*2];
            const double Q = buffer[si*2+1];
            const double mag = sqrt(I*I + Q*Q);
            if (mag > maxMag) maxMag = mag;
            outBuffer[si*2] = (int16_t) (I > 0.0 ? I * SHRT_MAX : I * SHRT_MIN);
            outBuffer[si*2+1] = (int16_t) (Q > 0.0 ? Q * SHRT_MAX : Q * SHRT_MIN);
        }
        const size_t bytesToWrite = 2 * samples * sizeof(int16_t);
        size_t bytesWritten = 0;
        while (bytesWritten < bytesToWrite) {
            ssize_t count = write(STDOUT_FILENO, ((uint8_t*)outBuffer.get()) + bytesWritten, bytesToWrite - bytesWritten);
            if (count < 0) {
                throw LMS::exception(std::string("write() failed with error ") + std::string(strerror(errno)));
            }
            bytesWritten += count;
        }

        if (autoGain && loopCount++ % 7 == 0) {
            normalizedGain /= maxMag;
            maxMag = 0.0;
            LMS::SetNormalizedGain(dev, LMS_CH_RX, 0, normalizedGain);
        }

        std::atomic_signal_fence(std::memory_order_acquire);
    }
}

int main(int argc, char **argv) {
    try {
        LimeLog::log(LimeLog::INFO, "Starting up. Grabbing Lime...");
        LMS::Open(&dev, NULL, NULL);
        LimeLog::log(LimeLog::INFO, "Created Lime. Squeezing Lime...");
        LMS::EnableChannel(dev, LMS_CH_RX, 0, true);
        LMS::EnableChannel(dev, LMS_CH_TX, 0, false);
        LMS::SetAntenna(dev, LMS_CH_RX, 0, LMS_PATH_LNAW);
        LMS::SetSampleRate(dev, 2.4e6, 8);
        LMS::SetNormalizedGain(dev, LMS_CH_RX, 0, normalizedGain);

        stream.reset(new lms_stream_t);
        stream->handle                 = 0;
        stream->isTx                   = false;
        stream->channel                = 0;
        stream->fifoSize               = 0x7FFFF;
        stream->throughputVsLatency    = 0.5f;
        stream->dataFmt                = lms_stream_t::LMS_FMT_F32;

        LMS::SetupStream(dev, stream.get());
        mainLoop(true);
    } catch (std::exception &e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        running = false;
    }

    LimeLog::log(LimeLog::INFO, "Exiting...");

    cleanup();

    LimeLog::log(LimeLog::INFO, "Goodbye!");

    return 0;
}
