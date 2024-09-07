#ifndef MS_COMMON_HPP
#define MS_COMMON_HPP

#include <iostream>

#define MS_NODISCARD [[nodiscard]]

#define MS_ASSERT(cond)                                                        \
    if (!(cond)) {                                                             \
        std::cerr << "Assertion failed on " << __FILE__ << ":" << __LINE__     \
                  << std::endl;                                                \
        std::terminate();                                                      \
    }

#endif // #ifndef MS_COMMON_HPP
