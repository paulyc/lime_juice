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

#include "lime_juice.h"

namespace io {
namespace github {
namespace paulyc {

WRAP_LMS_FUNCTION(int(lms_device_t**, const lms_info_str_t, void*), Open);
WRAP_LMS_FUNCTION(int(lms_device_t*), Close);
WRAP_LMS_FUNCTION(int(lms_device_t*), Init);
WRAP_LMS_FUNCTION(int(lms_device_t*, bool, size_t, bool), EnableChannel);
WRAP_LMS_FUNCTION(int(lms_device_t*, float_type, size_t), SetSampleRate);
WRAP_LMS_FUNCTION(int(lms_device_t*, bool, size_t, float_type), SetLOFrequency);
WRAP_LMS_FUNCTION(int(lms_device_t*, bool, size_t, size_t), SetAntenna);
WRAP_LMS_FUNCTION(int(lms_device_t*, bool, size_t, unsigned), SetGaindB);
WRAP_LMS_FUNCTION(int(lms_device_t*, bool, size_t, float_type), SetNormalizedGain);
WRAP_LMS_FUNCTION(int(lms_device_t*, lms_stream_t*), SetupStream);
WRAP_LMS_FUNCTION(int(lms_device_t*, lms_stream_t*), DestroyStream);
WRAP_LMS_FUNCTION(int(lms_stream_t*), StartStream);
WRAP_LMS_FUNCTION(int(lms_stream_t*), StopStream);
WRAP_LMS_FUNCTION(int(lms_stream_t*, void*, size_t, lms_stream_meta_t*, unsigned), RecvStream);

} // namespace paulyc
} // namespace github
} // namespace io
