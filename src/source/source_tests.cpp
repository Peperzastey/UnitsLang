#include "Source.h"
#include <array>
#include <gtest/gtest.h>
#include <iostream>

const std::string MOCK_SOURCE_STR = "abc\nd\nefg\nhij";

class MockSource : public Source {
public:
    MockSource(const std::string &srcStr = MOCK_SOURCE_STR)
        : chars_(srcStr) {}

    char provideChar() {
        if (pos_ >= chars_.size()) {
            return EOF;
        }
        return chars_[pos_++];
    }

    const std::string& getWholeString() const {
        return chars_;
    }

private:
    const std::string chars_;
    std::size_t pos_ = 0;
};

bool operator==(const PosInStream &lhs, const PosInStream &rhs) {
    return lhs.lineNumber == rhs.lineNumber && lhs.posInLine == rhs.posInLine;
}

TEST(SourceTests, GetUngetGetSequence) {
    MockSource mock;
    Source *src = &mock;

    char c = src->getChar();
    ASSERT_EQ(mock.getWholeString()[0], c);
    char c2 = src->getChar();
    ASSERT_EQ(mock.getWholeString()[1], c2);
    c = src->getChar();
    ASSERT_EQ(mock.getWholeString()[2], c);

    src->ungetChar(c);
    src->ungetChar(c2);

    c = src->getChar();
    ASSERT_EQ(mock.getWholeString()[1], c);
    c = src->getChar();
    ASSERT_EQ(mock.getWholeString()[2], c);
}

TEST(SourceTests, GetCurrentPositionReturnsCorrectPos) {
    MockSource mock("abc\nd\nefg\nhij");
    Source *src = &mock;

    ASSERT_EQ(src->getCurrentPosition(), (PosInStream{1, 0}));
    for (unsigned int i : {1, 2, 3, 4}) {
        src->getChar();
        ASSERT_EQ(src->getCurrentPosition(), (PosInStream{1, i}));
    }

    src->ungetChar(mock.getWholeString()[3]);
    src->getChar();
    ASSERT_EQ(src->getCurrentPosition(), (PosInStream{1, 4}));

    for (unsigned int i : {1, 2}) {
        src->getChar();
        ASSERT_EQ(src->getCurrentPosition(), (PosInStream{2, i}));
    }
    for (unsigned int i : {1, 2, 3, 4}) {
        src->getChar();
        ASSERT_EQ(src->getCurrentPosition(), (PosInStream{3, i}));
    }
    for (unsigned int i : {1, 2, 3}) {
        src->getChar();
        ASSERT_EQ(src->getCurrentPosition(), (PosInStream{4, i}));
    }
}
