#ifndef MS_COMMON_HPP
#define MS_COMMON_HPP

#include <iostream>

#include <fmt/format.h>

#define MS_NODISCARD [[nodiscard]]

#define MS_ASSERT(cond)                                                        \
    if (!(cond)) {                                                             \
        std::cerr << "Assertion failed on " << __FILE__ << ":" << __LINE__     \
                  << std::endl;                                                \
        std::terminate();                                                      \
    }

namespace ms {

struct IFmtNoParseFormatter {
    constexpr fmt::format_parse_context::iterator
    parse(fmt::format_parse_context &ctx) {
        return ctx.begin();
    }
};

} // namespace ms

#endif // #ifndef MS_COMMON_HPP
