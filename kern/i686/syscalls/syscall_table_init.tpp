/*
syscalls_init.tpp

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

namespace SyscallType
{
enum SyscallType
{
    LudOS = 0,
    Linux = 1
};
}

template <size_t Idx, typename Tuple>
void get_argument(const registers* const regs, Tuple& tuple)
{
    decltype(regs->eax) value;
    if constexpr (Idx == 0)
    {
        value = regs->ebx;
    }
    else if constexpr (Idx == 1)
    {
        value = regs->ecx;
    }
    else if constexpr (Idx == 2)
    {
        value = regs->edx;
    }
    else if constexpr (Idx == 3)
    {
        value = regs->esi;
    }
    else if constexpr (Idx == 4)
    {
        value = regs->edi;
    }
    else if constexpr (Idx == 5)
    {
        value = regs->ebp;
    }

    std::get<Idx>(tuple) = (std::tuple_element_t<Idx, std::remove_reference_t<decltype(tuple)>>)value;
}

template <size_t... I, typename Tuple>
void assign_values_impl(const registers* const regs, Tuple& tuple, std::index_sequence<I...>)
{
    (get_argument<I>(regs, tuple), ...);
}

template <typename... Args>
void assign_values(const registers* const regs, std::tuple<Args...>& tuple)
{
    assign_values_impl(regs, tuple, std::index_sequence_for<Args...>{});
}

template <SyscallType::SyscallType type, size_t Number, typename ReturnType, typename... Args>
void add_syscall(ReturnType(*target)(Args...))
{
    static_assert(Number < max_syscalls);
    static_assert(sizeof...(Args) <= 6, "x86 syscalls cannot take more than 6 parameters");

    auto& table = (type == SyscallType::LudOS) ? ludos_syscall_table : linux_syscall_table;

    table[Number].ptr = [target](const registers* const regs)
    {
        std::tuple<Args...> elements;
        assign_values(regs, elements);
        if constexpr (std::is_same_v<ReturnType, void>)
        {
            std::apply(target, elements);
            return EOK;
        }
        else
        {
            static_assert(std::is_convertible_v<ReturnType, syscall_ptr::result_type>);

            return (syscall_ptr::result_type)std::apply(target, elements);
        }
    };

    table[Number].arg_cnt = sizeof...(Args);

    size_t i { 0 };
    ((table[Number].arg_sizes[i++] = sizeof(Args)), ...);
}
