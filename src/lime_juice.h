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

#ifndef _COM_GITHUB_PAULYC_LIME_JUICE_LIME_JUICE_H_
#define _COM_GITHUB_PAULYC_LIME_JUICE_LIME_JUICE_H_

#include <string>
#include <functional>

#include <lime/LimeSuite.h>

#include "logger.h"

#define WRAP_LMS_FUNCTION(signature, func) \
std::function<signature> LMS::func  = \
LMS::wrap_error_check<>(std::function<signature>(LMS_ ## func));

namespace io {
namespace github {
namespace paulyc {

class LMS {
public:
    template <typename R, typename... Args>
    static std::function<R(Args...)> wrap_error_check(std::function<R(Args...)> fun) {
        return [fun](Args... args) -> R {
            R res = fun(args...);
            if (res == -1) {
                std::string msg("Error calling some fun: ");
                msg += LMS_GetLastErrorMessage();
                LimeLog::log(LimeLog::ERROR, msg.c_str());
                throw LMS::exception();
            }
            return res;
        };
    }

    static std::function<int(lms_info_str_t*)> GetDeviceList;
    static std::function<int(lms_device_t**, const lms_info_str_t, void*)> Open;
    static std::function<int(lms_device_t*)> Close;
    static std::function<int(lms_device_t*)> Init;
    static std::function<int(lms_device_t*, bool dir_tx, size_t, bool)> EnableChannel;
    static std::function<int(lms_device_t*, float_type, size_t)> SetSampleRate;
    static std::function<int(lms_device_t*, bool, size_t, float_type)> SetLOFrequency;
    static std::function<int(lms_device_t*, bool, size_t, size_t)> SetAntenna;
    static std::function<int(lms_device_t*, bool, size_t, unsigned)> SetGaindB;
    static std::function<int(lms_device_t*, lms_stream_t*)> SetupStream;
    static std::function<int(lms_device_t*, lms_stream_t*)> DestroyStream;
    static std::function<int(lms_stream_t*)> StartStream;
    static std::function<int(lms_stream_t*)> StopStream;
    static std::function<int(lms_stream_t*, void*, size_t, lms_stream_meta_t*, unsigned)> RecvStream;

    class exception : public std::exception {
    public:
        exception() : _msg("") {}
        exception(std::string msg) : _msg(msg) {}
        const char* what() const noexcept { return _msg.c_str(); }
    private:
        std::string _msg;
    };
};

} // namespace github
} // namespace paulyc
} // namespace com



#endif // _COM_GITHUB_PAULYC_LIME_JUICE_LIME_JUICE_H_
