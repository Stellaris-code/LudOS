/*
process.tpp

Copyright (c) 11 Yann BOUCHER (yann)

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

template <typename... Args, size_t... Idx>
void apply_vec(std::tuple<Args...>& t, const std::vector<uintptr_t>& vec, std::index_sequence<Idx...>) {
    ((std::get<Idx>(t) = (typename std::tuple_element<Idx, std::tuple<Args...>>::type)vec[Idx]), ...);
}

template <typename... Args>
uintptr_t Process::create_user_callback(const std::function<int(Args...)>& callback)
{
    std::vector<size_t> arg_sizes(sizeof...(Args));

    size_t i { 0 };
    ((arg_sizes[i++] = sizeof(Args)), ...);

    return create_user_callback_impl([callback](const std::vector<uintptr_t>& args)->int
    {
        std::tuple<Args...> tuple;
        apply_vec(tuple, args, std::make_index_sequence<sizeof...(Args)>());

        return std::apply(callback, tuple);
    }, arg_sizes);
}
