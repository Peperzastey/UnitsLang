#include "Source.h"
#include <array>
#include <gtest/gtest.h>
#include <iostream>

const std::string MOCK_SOURCE_STR = "abcdefghij";

class MockSource : public Source {
public:
    char provideChar() {
        if (pos_ >= chars_.size()) {
            return EOF;
        }
        return chars_[pos_++];
    }

private:
    const std::string &chars_ = MOCK_SOURCE_STR;
    std::size_t pos_ = 0;
};

TEST(SourceTests, GetUngetGetSequence) {
    MockSource mock;
    Source *src = &mock;

    char c = src->getChar();
    ASSERT_EQ(MOCK_SOURCE_STR[0], c);
    char c2 = src->getChar();
    ASSERT_EQ(MOCK_SOURCE_STR[1], c2);
    c = src->getChar();
    ASSERT_EQ(MOCK_SOURCE_STR[2], c);

    src->ungetChar(c);
    src->ungetChar(c2);

    c = src->getChar();
    ASSERT_EQ(MOCK_SOURCE_STR[1], c);
    c = src->getChar();
    ASSERT_EQ(MOCK_SOURCE_STR[2], c);
}
