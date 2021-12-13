#include "Source.h"
#include "StringSource.h"
#include <gtest/gtest.h>

bool operator==(const PosInStream &lhs, const PosInStream &rhs) {
    return lhs.lineNumber == rhs.lineNumber && lhs.posInLine == rhs.posInLine;
}

TEST(SourceTests, GetUngetGetSequence) {
    std::string testString = "abc\nd\nefg\nhij";
    StringSource strSrc(testString);
    Source *src = &strSrc;

    char c = src->getChar();
    ASSERT_EQ(testString[0], c);
    char c2 = src->getChar();
    ASSERT_EQ(testString[1], c2);
    c = src->getChar();
    ASSERT_EQ(testString[2], c);

    src->ungetChar(c);
    src->ungetChar(c2);

    c = src->getChar();
    ASSERT_EQ(testString[1], c);
    c = src->getChar();
    ASSERT_EQ(testString[2], c);
}

TEST(SourceTests, GetCurrentPositionReturnsCorrectPos) {
    std::string testString = "abc\nd\nefg\nhij";
    StringSource strSrc(testString);
    Source *src = &strSrc;

    ASSERT_EQ(src->getCurrentPosition(), (PosInStream{1, 0}));
    for (unsigned int i : {1, 2, 3, 4}) {
        src->getChar();
        ASSERT_EQ(src->getCurrentPosition(), (PosInStream{1, i}));
    }

    src->ungetChar(testString[3]);
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
