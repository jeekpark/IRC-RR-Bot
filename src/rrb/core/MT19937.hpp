#pragma once

#include <vector>

class MT19937
{
public:
    MT19937(unsigned long seed)
    : mState(N)
    , mIndex(N + 1)
    {
        mState[0] = seed;
        for (int i = 1; i < N; ++i)
            mState[i] = (F * (mState[i - 1] ^ (mState[i - 1] >> (W - 2))) + i);
    }
    MT19937(const MT19937& copy)
    : mState(copy.mState)
    , mIndex(copy.mIndex)
    {
    }
    MT19937& operator=(const MT19937& copy)
    {
        if (this != &copy)
        {
            mState = copy.mState;
            mIndex = copy.mIndex;
        }
        return *this;
    }
    virtual ~MT19937()
    {
    }
    unsigned long GetRandom()
    {
        if (mIndex >= N) twist();
        unsigned long y = mState[mIndex];
        y = y ^ ((y >> U) & D);
        y = y ^ ((y << S) & B);
        y = y ^ ((y << T) & C);
        y = y ^ (y >> L);
        mIndex = (mIndex + 1) % N;
        return y;
    }
private:
    void twist()
    {
        for (int i = 0; i < N; ++i)
        {
            unsigned long x = (mState[i] & 0x80000000) + (mState[(i + 1) % N] & 0x7FFFFFFF);
            unsigned long xA = x >> 1;
            if (x % 2 != 0)
                xA ^= A;
            mState[i] = mState[(i + M) % N] ^ xA;
        }
        mIndex = 0;
    }
    static const int W = 32, N = 624, M = 397, R = 31;
    static const unsigned long A = 0x9908B0DF, F = 1812433253;
    static const int U = 11, S = 7, T = 15, L = 18;
    static const unsigned long D = 0xFFFFFFFF, B = 0x9D2C5680, C = 0xEFC60000;
    std::vector<unsigned long> mState;
    int mIndex;
};
