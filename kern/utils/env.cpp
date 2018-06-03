/*
env.cpp

Copyright (c) 29 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "env.hpp"

#include "utils/stlutils.hpp"
#include "utils/logging.hpp"
#include "utils/messagebus.hpp"

std::unordered_map<kpp::string, kpp::string> kenv;

void read_from_cmdline(const kpp::string& cmdline)
{
    kenv.clear();

    for (const auto& entry : tokenize(cmdline, " ", true))
    {
        auto values = tokenize(entry, "=");
        if (values.size() == 1) values.emplace_back("");
        ksetenv(values[0], values[1]);
    }
}

kpp::optional<kpp::string> kgetenv(kpp::string s)
{
    if (kenv.find(s) != kenv.end())
    {
        return kenv[s];
    }
    else
    {
        return {};
    }
}

void ksetenv(const kpp::string &key, kpp::string val)
{
    kenv[key] = val;
    MessageBus::send<EnvVarChange>({key, val});
}
