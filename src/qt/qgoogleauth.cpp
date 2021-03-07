#include "qgoogleauth.h"
#include <QtEndian>
#include <QDateTime>
#include <QCryptographicHash>
#include <QSettings>

QGoogleAuth::QGoogleAuth()
{
}

QByteArray QGoogleAuth::hmacSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64;             // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "\"
    // ascii characters 0x36 ("6") and 0x5c ("\") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed;
}

int QGoogleAuth::base32_decode(const quint8* encoded, quint8* result, int bufSize)
{
    int buffer = 0;
    int bitsLeft = 0;
    int count = 0;

    for (const quint8* ptr = encoded; count < bufSize && *ptr; ++ptr) {
        quint8 ch = *ptr;

        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '-') {
            continue;
        }

        buffer <<= 5;

        // Deal with commonly mistyped characters
        if (ch == '0') {
            ch = 'O';
        } else if (ch == '1') {
            ch = 'L';
        } else if (ch == '8') {
            ch = 'B';
        }

        // Look up one base32 digit
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            ch = (ch & 0x1F) - 1;
        } else if (ch >= '2' && ch <= '7') {
            ch -= '2' - 26;
        } else {
            return -1;
        }

        buffer |= ch;
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            result[count++] = buffer >> (bitsLeft - 8);
            bitsLeft -= 8;
        }
    }

    if (count < bufSize) {
        result[count] = '\000';
    }

    return count;
}

QString QGoogleAuth::generatePin(const QByteArray key)
{
    quint64 time = QDateTime::currentDateTime().toTime_t();
    quint64 current = qToBigEndian(time / 30);

    int secretLen = (key.length() + 7) / 8 * 5;
    quint8 secret[100];
    int res = base32_decode(reinterpret_cast<const quint8*>(key.constData()), secret, secretLen);
    QByteArray hmac = hmacSha1(QByteArray(reinterpret_cast<const char*>(secret), res), QByteArray((char*)&current, sizeof(current)));

    int offset = (hmac[hmac.length() - 1] & 0xf);
    int binary =
        ((hmac[offset] & 0x7f) << 24) | ((hmac[offset + 1] & 0xff) << 16) | ((hmac[offset + 2] & 0xff) << 8) | (hmac[offset + 3] & 0xff);
    QSettings settings;
    int digits = settings.value("2fadigits").toInt();
    int password;
    if (digits == 8) {
        password = binary % 100000000;
        return QString("%1").arg(password, 8, 10, QChar('0'));
    } else if (digits == 6) {
        password = binary % 1000000;
        return QString("%1").arg(password, 6, 10, QChar('0'));
    }
}
