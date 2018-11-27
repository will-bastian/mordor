// Copyright (c) 2010 - Mozy, Inc.

#include "mordor/string.h"
#include "mordor/test/test.h"

using namespace Mordor;

MORDOR_UNITTEST(String, dataFromHexstring)
{
    MORDOR_TEST_ASSERT_EQUAL(dataFromHexstring(""), "");
    MORDOR_TEST_ASSERT_EQUAL(dataFromHexstring("00"), std::string("\0", 1));
    MORDOR_TEST_ASSERT_EQUAL(dataFromHexstring("abcd"), "\xab\xcd");
    MORDOR_TEST_ASSERT_EQUAL(dataFromHexstring("01eF"), "\x01\xef");
    MORDOR_TEST_ASSERT_EXCEPTION(dataFromHexstring("0"),
        std::invalid_argument);
    MORDOR_TEST_ASSERT_EXCEPTION(dataFromHexstring("fg"),
        std::invalid_argument);
    MORDOR_TEST_ASSERT_EXCEPTION(dataFromHexstring("fG"),
        std::invalid_argument);
    MORDOR_TEST_ASSERT_EXCEPTION(dataFromHexstring(std::string("\0\0", 2)),
        std::invalid_argument);
}

MORDOR_UNITTEST(String, sha0sum)
{
    MORDOR_TEST_ASSERT_EQUAL(hexstringFromData(sha0sum("")), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    MORDOR_TEST_ASSERT_EQUAL(hexstringFromData(sha0sum("1234567890")), "01b307acba4f54f55aafc33bb06bbbf6ca803e9a");
    MORDOR_TEST_ASSERT_EQUAL(hexstringFromData(sha0sum((const void *)"\x7e\x54\xe4\xbc\x27\x00\x40\xab", 8)), "f3ac229ecb812dc70a45bb901de053ffea00d773");
}

MORDOR_UNITTEST(String, validateUtf8)
{
    MORDOR_TEST_ASSERT(validateUtf8(std::string("hello")));
    MORDOR_TEST_ASSERT(!validateUtf8(std::string("\xc0\xc1", 2)));
    MORDOR_TEST_ASSERT(!validateUtf8(std::string("\0\xc0\0\xc1", 4)));
    MORDOR_TEST_ASSERT(!validateUtf8(std::string("\xf0\xa7\xad", 3)));
    MORDOR_TEST_ASSERT(validateUtf8(std::string("\xf0\xa7\xad\xa2", 4)));
    MORDOR_TEST_ASSERT(validateUtf8(std::string("\xfb\xbf\xbf\xbf\xbf", 5)));
    MORDOR_TEST_ASSERT(!validateUtf8(std::string("\xf8\x80\x80\x80\x80", 5)));
    MORDOR_TEST_ASSERT(validateUtf8(std::string("\xfd\xbf\xbf\xbf\xbf\xbf", 6)));
    MORDOR_TEST_ASSERT(!validateUtf8(std::string("\xfc\x80\x80\x80\x80\x80", 6)));
    MORDOR_TEST_ASSERT(validateUtf8(std::string("\xfd\xbf\xbf\xbf\xbf\xbf"
                                                         "\xfb\xbf\xbf\xbf\xbf"
                                                         "\xf0\xa7\xad\xa2"
                                                         "\xe8\x80\x80"
                                                         "\xd0\xb0"
                                                         "\0", 6 + 5 + 4 + 3 + 2 + 1)));
}

