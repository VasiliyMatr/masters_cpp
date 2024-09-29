#ifndef STRING_EQ_HPP
#define STRING_EQ_HPP

#include <string>

namespace ms {

template <class CharT, class Traits, class Allocator_1, class Allocator_2>
bool operator==(const std::basic_string<CharT, Traits, Allocator_1> &lhs,
                const std::basic_string<CharT, Traits, Allocator_2> &rhs) {
    return lhs.compare(rhs) == 0;
}

template <class CharT, class Traits, class Allocator>
bool operator==(const CharT *lhs,
                const std::basic_string<CharT, Traits, Allocator> &rhs) {
    return rhs.compare(lhs) == 0;
}

template <class CharT, class Traits, class Allocator>
bool operator==(const std::basic_string<CharT, Traits, Allocator> &lhs,
                const CharT *rhs) {
    return lhs.compare(rhs) == 0;
}

} // namespace ms

#endif // STRING_EQ_HPP
