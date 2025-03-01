/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2016 Intel Corporation.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QString>

#include <qtest.h>
#include <limits>

class tst_QByteArray : public QObject
{
    Q_OBJECT
    QByteArray sourcecode;
private slots:
    void initTestCase();
    void append();
    void append_data();

    void toLongLong_data();
    void toLongLong();
    void toULongLong_data();
    void toULongLong();

    void latin1Uppercasing_qt54();
    void latin1Uppercasing_xlate();
    void latin1Uppercasing_xlate_checked();
    void latin1Uppercasing_category();
    void latin1Uppercasing_bitcheck();

    void toPercentEncoding_data();
    void toPercentEncoding();
};

void tst_QByteArray::initTestCase()
{
    QFile self(QFINDTESTDATA("tst_bench_qbytearray.cpp"));
    QVERIFY(self.open(QIODevice::ReadOnly));
    sourcecode = self.readAll();
}

void tst_QByteArray::append_data()
{
    QTest::addColumn<int>("size");
    QTest::newRow("1")         << int(1);
    QTest::newRow("10")        << int(10);
    QTest::newRow("100")       << int(100);
    QTest::newRow("1000")      << int(1000);
    QTest::newRow("10000")     << int(10000);
    QTest::newRow("100000")    << int(100000);
    QTest::newRow("1000000")   << int(1000000);
    QTest::newRow("10000000")  << int(10000000);
    QTest::newRow("100000000") << int(100000000);
}

void tst_QByteArray::append()
{
    QFETCH(int, size);

    QByteArray ba;
    QBENCHMARK {
        QByteArray ba2(size, 'x');
        ba.append(ba2);
        ba.clear();
    }
}

static QByteArray decNext(QByteArray &&big)
{
    // Increments a decimal digit-string (ignoring sign, so decrements if
    // negative); only intended for taking a boundary value just out of range,
    // so big is never a string of only 9s (that'd be one less than a power of
    // ten, which cannot be a power of two, as odd, or one less than one, as the
    // power of ten isn't a power of two).
    int i = big.size() - 1;
    while (big.at(i) == '9')
        big[i--] = '0';
    big[i] += 1;
    return big;
}

void tst_QByteArray::toLongLong_data()
{
    QTest::addColumn<QByteArray>("text");
    QTest::addColumn<bool>("good");
    QTest::addColumn<qlonglong>("number");
#define ROW(n) QTest::newRow(#n) << QByteArray(#n) << true << n ## LL
    ROW(0);
    ROW(1);
    ROW(-1);
    ROW(17);
    ROW(-17);
    ROW(1234567890);
    ROW(-1234567890);
#undef ROW
    using LL = std::numeric_limits<qlonglong>;
    QTest::newRow("min") << QByteArray::number(LL::min()) << true << LL::min();
    QTest::newRow("min-1") << decNext(QByteArray::number(LL::min())) << false << 0LL;
    QTest::newRow("max") << QByteArray::number(LL::max()) << true << LL::max();
    QTest::newRow("max+1") << decNext(QByteArray::number(LL::max())) << false << 0LL;
}

void tst_QByteArray::toLongLong()
{
    QFETCH(QByteArray, text);
    QFETCH(bool, good);
    QFETCH(qlonglong, number);

    qlonglong actual = 0;
    bool ok;
    QBENCHMARK {
        actual = text.toLongLong(&ok);
    }
    QCOMPARE(actual, number);
    QCOMPARE(ok, good);
}

void tst_QByteArray::toULongLong_data()
{
    QTest::addColumn<QByteArray>("text");
    QTest::addColumn<bool>("good");
    QTest::addColumn<qulonglong>("number");
#define ROW(n) \
    QTest::newRow(#n) << QByteArray(#n) << true << n ## ULL; \
    QTest::newRow("-" #n) << QByteArray("-" #n) << false << 0ULL
    ROW(0);
    ROW(1);
    ROW(17);
    ROW(1234567890);
#undef ROW
    using ULL = std::numeric_limits<qulonglong>;
    QTest::newRow("max") << QByteArray::number(ULL::max()) << true << ULL::max();
    QTest::newRow("max+1") << decNext(QByteArray::number(ULL::max())) << false << 0ULL;
}

void tst_QByteArray::toULongLong()
{
    QFETCH(QByteArray, text);
    QFETCH(bool, good);
    QFETCH(qulonglong, number);

    qulonglong actual = 0;
    bool ok;
    QBENCHMARK {
        actual = text.toULongLong(&ok);
    }
    QCOMPARE(actual, number);
    QCOMPARE(ok, good);
}

void tst_QByteArray::latin1Uppercasing_qt54()
{
    QByteArray s = sourcecode;
    s.detach();

    // the following was copied from qbytearray.cpp (except for the QBENCHMARK macro):
    uchar *p_orig = reinterpret_cast<uchar *>(s.data());
    uchar *e = reinterpret_cast<uchar *>(s.end());

    QBENCHMARK {
        uchar *p = p_orig;
        if (p) {
            while (p != e) {
                *p = QChar::toLower((ushort)*p);
                p++;
            }
        }
    }
}


/*
#!/usr/bin/perl -l
use feature "unicode_strings"
for (0..255) {
    $up = uc(chr($_));
    $up = chr($_) if ord($up) > 0x100 || length $up > 1;
    printf "0x%02x,", ord($up);
    print "" if ($_ & 0xf) == 0xf;
}
*/
static const uchar uppercased[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xf7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xff
};
void tst_QByteArray::latin1Uppercasing_xlate()
{
    QByteArray output = sourcecode;
    output.detach();
    char *dst_orig = output.data();
    const char *src_orig = sourcecode.constBegin();
    const char *end = sourcecode.constEnd();
    QBENCHMARK {
        char *dst = dst_orig;
        for (const char *src = src_orig; src != end; ++src, ++dst)
            *dst = uppercased[uchar(*src)];
    }
}

void tst_QByteArray::latin1Uppercasing_xlate_checked()
{
    QByteArray output = sourcecode;
    output.detach();
    char *dst_orig = output.data();
    const char *src_orig = sourcecode.constBegin();
    const char *end = sourcecode.constEnd();
    QBENCHMARK {
        char *dst = dst_orig;
        for (const char *src = src_orig; src != end; ++src, ++dst) {
            uchar ch = uchar(*src);
            uchar converted = uppercased[ch];
            if (ch != converted)
                *dst = converted;
        }
    }
}

/*
#!/bin/perl -l
use feature "unicode_strings";
sub categorize($) {
    # 'ß' and 'ÿ' are lowercase, but we cannot uppercase them
    return 0 if $_[0] == 0xDF || $_[0] == 0xFF;
    $ch = chr($_[0]);
    return 2 if uc($ch) ne $ch;
    return 1 if lc($ch) ne $ch;
    return 0;
}
for (0..255) {
    printf "%d,", categorize($_);
    print "" if ($_ & 0xf) == 0xf;
}
*/
static const char categories[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,0,2,2,2,2,2,2,2,0
};

void tst_QByteArray::latin1Uppercasing_category()
{
    QByteArray output = sourcecode;
    output.detach();
    char *dst_orig = output.data();
    const char *src_orig = sourcecode.constBegin();
    const char *end = sourcecode.constEnd();
    QBENCHMARK {
        char *dst = dst_orig;
        for (const char *src = src_orig; src != end; ++src, ++dst)
            *dst = categories[uchar(*src)] == 1 ? *src & ~0x20 : *src;
    }
}

/*
#!/bin/perl -l
use feature "unicode_strings";
sub categorize($) {
    # 'ß' and 'ÿ' are lowercase, but we cannot uppercase them
    return 0 if $_[0] == 0xDF || $_[0] == 0xFF;
    $ch = chr($_[0]);
    return 2 if uc($ch) ne $ch;
    return 1 if lc($ch) ne $ch;
    return 0;
}
for $row (0..7) {
    $val = 0;
    for $col (0..31) {
        $val |= (1<<$col)
            if categorize($row * 31 + $col) == 2;
    }
    printf "0x%08x,", $val;
}
*/

static const quint32 shouldUppercase[8] = {
    0x00000000,0x00000000,0x00000000,0x3ffffff0,0x00000000,0x04000000,0x00000000,0xbfffff80
};

static bool bittest(const quint32 *data, uchar bit)
{
    static const unsigned bitsperelem = sizeof(*data) * CHAR_BIT;
    return data[bit / bitsperelem] & (1 << (bit & (bitsperelem - 1)));
}

void tst_QByteArray::latin1Uppercasing_bitcheck()
{
    QByteArray output = sourcecode;
    output.detach();
    char *dst_orig = output.data();
    const char *src_orig = sourcecode.constBegin();
    const char *end = sourcecode.constEnd();
    QBENCHMARK {
        char *dst = dst_orig;
        for (const char *src = src_orig; src != end; ++src, ++dst)
            *dst = bittest(shouldUppercase, *src) ? uchar(*src) & ~0x20 : uchar(*src);
    }
}

void tst_QByteArray::toPercentEncoding_data()
{
    QTest::addColumn<QByteArray>("plaintext");
    QTest::addColumn<QByteArray>("expected");

    QTest::newRow("empty") << QByteArray("") << QByteArray("");
    QTest::newRow("plain")
        << QByteArray("the quick brown fox jumped over the lazy dogs")
        << QByteArray("the%20quick%20brown%20fox%20jumped%20over%20the%20lazy%20dogs");
    QTest::newRow("specials")
        << QByteArray(
            "\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11\x12\x13\x14\x15"
            "\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !\"#$%&'()*+,/:;<=>?@[\\]^`{|}\x7f")
        << QByteArray(
            "%01%02%03%04%05%06%07%08%09%0A%0B%0C%0D%0E%0F%10%11%12%13%14%15%16%17%18"
            "%19%1A%1B%1C%1D%1E%1F%20%21%22%23%24%25%26%27%28%29%2A%2B%2C%2F%3A%3B%3C"
            "%3D%3E%3F%40%5B%5C%5D%5E%60%7B%7C%7D%7F");
}

void tst_QByteArray::toPercentEncoding()
{
    QFETCH(QByteArray, plaintext);
    QByteArray encoded;
    QBENCHMARK {
        encoded = plaintext.toPercentEncoding();
    }
    QTEST(encoded, "expected");
}

QTEST_MAIN(tst_QByteArray)

#include "tst_bench_qbytearray.moc"
