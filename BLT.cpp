#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <random>
#include <cassert>
#include <cstring>
#include "./smhasher/src/MurmurHash3.h"
#include "./smhasher/src/MurmurHash3.cpp"

using namespace std;

struct Listed {
    vector<string> keys;
    vector<int64_t> values;
};

struct Entries {
    Listed Alice;
    Listed Bob;
};

class BloomLookupTable {
private:
    int64_t n, m, k;
    double denom;
    int64_t string_size;
    vector<int64_t> count;
    vector<string> keyxor;
    vector<int64_t> valuexor;

    string str_xor(string& str1, string& str2) {
        string x_xor(string_size, ' ');
        
        transform(str1.begin(), str1.end(), str2.begin(), x_xor.begin(),
                [](char c1, char c2){ return c1 ^ c2; });
        
        return x_xor;
    }

    uint64_t murmurhash(string& x_str, int64_t seed) {
        uint64_t hash_otpt[2];
        const char* x = x_str.data();
        
        MurmurHash3_x64_128(x, (uint64_t)(x_str.size()), seed, hash_otpt);
        
        return hash_otpt[1] % m;
    }


public:
    BloomLookupTable(int64_t n, int m, int k, int string_size) : n(n), m(m), k(k), string_size(string_size) {
        count.resize(m, 0);
        denom = (double)m / (double)n;
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuexor.resize(m, 0);
    }

    BloomLookupTable(int64_t n, double denom, int string_size) : n(n), m(denom * n), denom(denom), k(3), string_size(string_size) {
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuexor.resize(m, 0);
    }

    BloomLookupTableSubstracted& operator-=(const BloomLookupTable& other) {
        for (uint64_t i = 0; i < m; i++) {
            count[i] -= other.count[i];
            keyxor[i] = str_xor(keyxor[i], other.keyxor[i]);
            valuexor[i] ^= other.valuexor[i];
        }
        return *this;
    }

    virtual void insert(string x_str, int64_t y) {
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = murmurhash(x_str, seed);

            count[k_hash] += 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            valuexor[k_hash] ^= y;
        }
    }

    virtual void remove(string x_str, int64_t y) {
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = murmurhash(x_str, seed);

            count[k_hash] -= 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            valuexor[k_hash] ^= y;
        }
    }

    virtual int get(string x_str) { // -1 if there is no such key with high probability, -2 if it's 100%
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = murmurhash(x_str, seed);

            if (count[k_hash] == 0) {
                return -2;
            }
            if (count[k_hash] == 1 && keyxor[k_hash] == x) {
                return valuexor[k_hash];
            }
        }
        return -1;
    }

    virtual Listed list_entries() {
        Listed result;
        auto it = find(count.begin(), count.end(), 1);
        while (it != count.end()) {
            int64_t index = it - count.begin();
            result.keys.push_back(keyxor[index]);
            result.values.push_back(valuexor[index]);
            remove(keyxor[index], valuexor[index]);
            it = find(count.begin(), count.end(), 1);
        }
        return result;
    }
};


class BloomLookupTableSubstracted : public BloomLookupTable {
private:
    vector<int64_t> abs_count;
public: 
    void insert(string x_str, int64_t y) override {
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = murmurhash(x_str, seed);

            count[k_hash] += 1;
            abs_count[k_hash] += 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            valuexor[k_hash] ^= y;
        }
    }

    void remove(string x_str, int64_t y) override {
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = murmurhash(x_str, seed);

            count[k_hash] -= 1;
            abs_count[k_hash] -= 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            valuexor[k_hash] ^= y;
        }
    }

    int get(string x_str) override { // -1 if there is no such key with high probability, -2 if it's 100%
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = murmurhash(x_str, seed);

            if (abs_count[k_hash] == 0) {
                return -2;
            }
            if (abs_count[k_hash] == 1 && keyxor[k_hash] == x) {
                return valuexor[k_hash];
            }
        }
        return -1;
    }

 
    Entries list_entries() override {
        Entries result;
        auto it = find(abs_count.begin(), abs_count.end(), 1);
        while (it != count.end()) {
            int64_t index = it - abs_count.begin();
            if (count[index] == 1) {
                result.Alice.keys.push_back(keyxor[index]);
                result.Alice.values.push_back(valuesum[index]);
                remove(keyxor[index], valuesum[index]);
            } else {
                result.Bob.keys.push_back(keyxor[index]);
                result.Bob.values.push_back(valuesum[index]);
                insert(keyxor[index], valuesum[index]);
            }
            it = find(abs_count.begin(), abs_count.end(), 1);
        }
        return result;
    }
}
