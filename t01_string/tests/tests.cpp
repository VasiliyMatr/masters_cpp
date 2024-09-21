#include <gtest/gtest.h>

#include <cstring>

#include <ms/string.hpp>

using namespace std::literals::string_view_literals;

TEST(MsString, CowStringBasic) {
    ms::CowString s1{"Bar!"sv};
    ms::CowString s2 = s1;

    s2[2] = 'z';

    auto snp1 = s1.snp();
    auto snp2 = s2.snp();

    ASSERT_EQ("Bar!", snp1());
    ASSERT_EQ("Baz!", snp2());
}

TEST(MsString, CowStringPlus) {
    ms::CowString str1{" bar"sv};
    auto str2 = "foo"sv + str1 + str1 + " baz!"sv;

    auto snp1 = str1.snp();
    auto snp2 = str2.snp();

    ASSERT_EQ(" bar", snp1());
    ASSERT_EQ("foo bar bar baz!", snp2());
}

TEST(MsString, Tokenize) {
    ms::CowString str{"foo, bar, baz!"sv};

    auto delim = " ,!"sv;

    auto snp = str.snp();

    ms::Tokenizer t(snp(), delim);

    ASSERT_EQ("foo", t.next());
    ASSERT_EQ("bar", t.next());
    ASSERT_EQ("baz", t.next());
    ASSERT_EQ("", t.next());
}

TEST(MsString, CowStringSubStr) {
    ms::CowString str{"vd_kfjvndfind_meamsl_ck"sv};

    auto snp = str.snp();

    const auto *found = std::strstr(snp().data(), "find_me");
    ASSERT_STREQ(found, "find_meamsl_ck");
}
