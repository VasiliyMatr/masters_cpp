#ifndef TESTQUAL_HPP
#define TESTQUAL_HPP

#include <cctype>
#include <sstream>
#include <vector>

#include <ms/cow.hpp>

namespace ms {

enum class PType { kPointerTo, kArray, kUndef };
enum class PrimitiveType { kChar, kUndef };

std::string_view format_as(PType t) {
    switch (t) {
    case PType::kPointerTo:
        return "*";
    case PType::kArray:
        return "[]";
    default:
        return "<UNDEF>";
    }
}

std::string_view format_as(PrimitiveType t) {
    switch (t) {
    case PrimitiveType::kChar:
        return "char";
    default:
        return "<UNDEF>";
    }
}

class QualDecomposition final {
    friend fmt::formatter<ms::QualDecomposition>;

    bool m_valid = false;

    std::vector<bool> m_cv{};
    std::vector<PType> m_p{};

    PrimitiveType m_u = PrimitiveType::kUndef;

    QualDecomposition() = default;

  public:
    QualDecomposition(std::string_view str) {
        // Will collect reversed cv and p
        std::vector<bool> rcv{};
        std::vector<PType> rp{};

        /// [1] Preprocess input str
        std::string preproc{};
        preproc.reserve(str.size() * 3);

        bool found_space = false;
        for (auto c : str) {
            if (std::isspace(c)) {
                found_space = true;
                continue;
            }

            if (c == '*' || c == '[' || found_space) {
                preproc.push_back(';');
            }

            found_space = false;

            preproc.push_back(c);

            if (c == '*') {
                preproc.push_back(';');
            }
        }

        str = preproc;

        /// [2] Tokenize string
        Tokenizer tokenizer(str, std::string_view(";"));

        std::string_view curr_tok = tokenizer.next();

        rcv.push_back(false); // default CVn

        if (curr_tok == "const") {
            rcv.back() = true;
            curr_tok = tokenizer.next();
        }

        if (curr_tok != "char") {
            return; // error
        }

        m_u = PrimitiveType::kChar;

        /// [2.1] Check for all supported tokens
        while (!(curr_tok = tokenizer.next()).empty()) {
            if (curr_tok == "*") {
                rcv.push_back(false);
                rp.push_back(PType::kPointerTo);
                continue;
            }

            if (curr_tok == "[]") {
                // Propogate cv of array elements
                rcv.push_back(rcv.back());
                rp.push_back(PType::kArray);

                if (!tokenizer.next().empty()) {
                    return; // Not supported -> error
                }

                continue;
            }

            if (curr_tok == "const") {
                if (rcv.back()) {
                    return; // error
                }

                rcv.back() = true;
                continue;
            }

            return; // Unknown token -> error
        }

        // [3] Reverse cv & p

        m_cv.reserve(rcv.size());
        std::copy(rcv.rbegin(), rcv.rend(),
                  std::insert_iterator(m_cv, m_cv.begin()));

        m_p.reserve(rp.size());
        std::copy(rp.rbegin(), rp.rend(),
                  std::insert_iterator(m_p, m_p.begin()));

        m_valid = true;
    }

    MS_NODISCARD bool isValid() const noexcept { return m_valid; }

    MS_NODISCARD bool isSimilarTo(const QualDecomposition &snd) const noexcept {
        if (!isValid() || !snd.isValid()) {
            return false;
        }

        if (m_p.size() != snd.m_p.size() || m_u != snd.m_u) {
            return false;
        }

        for (size_t i = 0, end = m_p.size(); i != end; ++i) {
            if (m_p[i] != snd.m_p[i]) {
                return false;
            }
        }

        return true;
    }

    MS_NODISCARD bool isArray() const noexcept {
        return !m_p.empty() && m_p.front() == PType::kArray;
    }

    MS_NODISCARD bool
    hasSameCvSignatureAs(const QualDecomposition &snd) const noexcept {
        if (!isValid() || !snd.isValid()) {
            return false;
        }

        if (m_cv.size() != snd.m_cv.size()) {
            return false;
        }

        for (size_t i = 1, end = m_cv.size(); i != end; ++i) {
            if (m_cv[i] != snd.m_cv[i]) {
                return false;
            }
        }

        return true;
    }

    MS_NODISCARD QualDecomposition
    getQualCombined(const QualDecomposition &snd) const {
        QualDecomposition out{};

        if (m_cv.size() != snd.m_cv.size() || m_u != snd.m_u) {
            return out; // invalid
        }

        auto cv_size = m_cv.size();
        auto p_size = cv_size - 1;

        out.m_u = m_u;

        out.m_cv.reserve(cv_size);
        out.m_p.reserve(cv_size - 1);

        size_t const_add_id = 0;
        for (size_t i = 0; i != p_size; ++i) {
            // cv0 value is not specified by the standard. Will do the same
            out.m_cv.push_back(m_cv[i] || snd.m_cv[i]);

            bool is_array =
                m_p[i] == PType::kArray || snd.m_p[i] == PType::kArray;
            out.m_p.push_back(is_array ? PType::kArray : m_p[i]);

            if (m_cv[i] != snd.m_cv[i] || m_p[i] != snd.m_p[i]) {
                const_add_id = i;
            }
        }

        out.m_cv.push_back(m_cv.back() || snd.m_cv.back());
        if (m_cv.back() != snd.m_cv.back()) {
            const_add_id = cv_size - 1;
        }

        for (size_t i = 1; i < const_add_id; ++i) {
            out.m_cv[i] = true;
        }

        out.m_valid = true;

        // Should've already returned invalid otherwise
        MS_ASSERT(out.isSimilarTo(*this));

        return out;
    }

    void dump(std::ostream &o) const {
        if (!isValid()) {
            o << "<INVALID>";
            return;
        }

        for (size_t i = 0, end = m_p.size(); i != end; ++i) {
            if (m_cv[i]) {
                o << "const ";
            }

            switch (m_p[i]) {
            case PType::kPointerTo:
                o << "* ";
                break;
            case PType::kArray:
                o << "[] ";
                break;

            default:
                o << "<UNDEF> ";
                break;
            }
        }

        if (m_cv.back()) {
            o << "const ";
        }

        switch (m_u) {
        case PrimitiveType::kChar:
            o << "char";
            break;
        default:
            o << "<UNDEF>";
        }
    }
};

} // namespace ms

template <>
struct fmt::formatter<ms::QualDecomposition> : public ms::IFmtNoParseFormatter {
    fmt::format_context::iterator format(const ms::QualDecomposition &dec,
                                         fmt::format_context &ctx) const {
        auto out = ctx.out();

        if (!dec.isValid()) {
            return fmt::format_to(out, "<INVALID>");
        }

        for (size_t i = 0, end = dec.m_p.size(); i != end; ++i) {
            if (dec.m_cv[i]) {
                out = fmt::format_to(out, "const ");
            }

            out = fmt::format_to(out, "{} ", dec.m_p[i]);
        }

        if (dec.m_cv.back()) {
            out = fmt::format_to(out, "const ");
        }

        out = fmt::format_to(out, "{}", dec.m_u);

        return out;
    }
};

namespace ms {

inline bool testqual(const char *fst, const char *snd) {
    QualDecomposition fst_dec{fst};
    QualDecomposition snd_dec{snd};
    QualDecomposition comb = fst_dec.getQualCombined(snd_dec);

#if 0
    fmt::println("fst = {}", fst_dec);
    fmt::println("snd = {}", snd_dec);
    fmt::println("cmb = {}", comb);
#endif

    return !snd_dec.isArray() && comb.hasSameCvSignatureAs(snd_dec);
}

} // namespace ms

#endif // TESTQUAL_HPP
