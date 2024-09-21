#ifndef MS_STRING_HPP
#define MS_STRING_HPP

#include <algorithm>
#include <memory>
#include <span>
#include <string_view>

#include <ms/common.hpp>

namespace ms {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class Tokenizer final {
    using std_string_view = std::basic_string_view<CharT, Traits>;

    std_string_view m_str{};
    std_string_view m_delim{};

  public:
    Tokenizer(std_string_view str, std_string_view delim)
        : m_str(str), m_delim(delim) {}

    auto next() noexcept {
        auto sz = m_str.size();
        auto tok_begin = std::min(m_str.find_first_not_of(m_delim), sz);
        auto tok_end = std::min(m_str.find_first_of(m_delim, tok_begin), sz);

        auto out = m_str.substr(tok_begin, tok_end - tok_begin);
        m_str = m_str.substr(tok_end);

        return out;
    }
};

template <typename CharT, typename Traits = std::char_traits<CharT>>
class CowString;

// Immutable COW string proxy. Owenrship is shared with original string
template <typename CharT, typename Traits = std::char_traits<CharT>>
class CowSnapshot final {
    friend CowString<CharT, Traits>;

    size_t m_size = 0;
    std::shared_ptr<CharT[]> m_data{};

    explicit CowSnapshot(size_t size, std::shared_ptr<CharT[]> data)
        : m_size(size), m_data(std::move(data)) {}

  public:
    std::basic_string_view<CharT, Traits> operator()() &noexcept {
        return {m_data.get(), m_size};
    }
};

template <typename CharT, typename Traits> class CowString final {
    using std_string_view = std::basic_string_view<CharT, Traits>;
    using std_string = std::basic_string<CharT, Traits>;

    size_t m_size = 0;
    size_t m_capacity = 0;
    std::shared_ptr<CharT[]> m_data{};

    void copyData(size_t new_size, size_t new_capacity) {
        MS_ASSERT(m_size >= new_size);
        MS_ASSERT(new_capacity >= new_size);

        auto new_data = std::make_unique<CharT[]>(new_capacity);
        std::copy(m_data.get(), m_data.get() + new_size, new_data.get());

        m_size = new_size;
        m_capacity = new_capacity;
        m_data = std::move(new_data);
    }

    void copyDataIfNotUnique() {
        if (!m_data.unique()) {
            copyData(m_size, m_capacity);
        }
    }

    enum class ConcatType { kAppend, kPrepend };

    template <ConcatType concat_type> void concat(std_string_view str) {
        auto new_size = m_size + str.size();

        if (new_size <= m_capacity && m_data.unique()) {
            if constexpr (concat_type == ConcatType::kAppend) {
                std::copy(str.begin(), str.end(), m_data.get() + m_size);
            } else {
                for (size_t i = 1; i < m_size; ++i) {
                    m_data.get()[new_size - i] = m_data.get()[m_size - i];
                }
                std::copy(str.begin(), str.end(), m_data.get());
            }

            m_size = new_size;
            return;
        }

        static constexpr size_t kCapacityFactor = 2;
        auto new_capacity = new_size * kCapacityFactor;
        auto new_data = std::make_unique<CharT[]>(new_capacity);

        if constexpr (concat_type == ConcatType::kAppend) {
            std::copy(m_data.get(), m_data.get() + m_size, new_data.get());
            std::copy(str.begin(), str.end(), new_data.get() + m_size);
        } else {
            std::copy(str.begin(), str.end(), new_data.get());
            std::copy(m_data.get(), m_data.get() + m_size,
                      new_data.get() + str.size());
        }

        m_size = new_size;
        m_capacity = new_capacity;
        m_data = std::move(new_data);
    }

  public:
    explicit CowString(std_string_view str)
        : m_size(str.size()), m_capacity(m_size),
          m_data(std::make_unique<CharT[]>(m_capacity)) {
        std::copy(str.cbegin(), str.cend(), m_data.get());
    }

    MS_NODISCARD auto to_string() const {
        return std_string(m_data.get(), m_size);
    }

    MS_NODISCARD auto snp() const noexcept {
        return CowSnapshot<CharT, Traits>{m_size, m_data};
    }

    /// Default big-5

    CowString(const CowString &) = default;
    CowString(CowString &&) = default;
    CowString &operator=(const CowString &) = default;
    CowString &operator=(CowString &&) = default;
    ~CowString() = default;

    /// Getters

    MS_NODISCARD auto size() const noexcept { return m_size; }
    MS_NODISCARD auto capacity() const noexcept { return m_capacity; }
    MS_NODISCARD auto operator[](size_t id) const noexcept {
        return m_data[id];
    }

    /// Cow methods

    MS_NODISCARD auto &operator[](size_t id) {
        copyDataIfNotUnique();
        return m_data[id];
    }

    void append(std_string_view str) { concat<ConcatType::kAppend>(str); }

    void prepend(std_string_view str) { concat<ConcatType::kPrepend>(str); }

    auto operator+(const CowString &rhs) const {
        auto lhs_copy = *this;
        auto rhs_snp = rhs.snp();
        lhs_copy.append(rhs_snp());
        return lhs_copy;
    }
};

template <typename CharT, typename Traits>
CowString<CharT, Traits> operator+(const CowString<CharT, Traits> &lhs,
                                   std::basic_string_view<CharT, Traits> rhs) {
    auto lhs_copy = lhs;
    lhs_copy.append(rhs);
    return lhs_copy;
}

template <typename CharT, typename Traits>
CowString<CharT, Traits> operator+(CowString<CharT, Traits> &&lhs,
                                   std::basic_string_view<CharT, Traits> rhs) {
    lhs.append(rhs);
    return std::move(lhs);
}

template <typename CharT, typename Traits>
CowString<CharT, Traits> operator+(std::basic_string_view<CharT, Traits> lhs,
                                   const CowString<CharT, Traits> &rhs) {
    auto rhs_copy = rhs;
    rhs_copy.prepend(lhs);
    return rhs_copy;
}

template <typename CharT, typename Traits>
CowString<CharT, Traits> operator+(std::basic_string_view<CharT, Traits> lhs,
                                   CowString<CharT, Traits> &&rhs) {
    rhs.prepend(lhs);
    return std::move(rhs);
}

} // namespace ms

#endif // #ifndef MS_STRING_HPP
