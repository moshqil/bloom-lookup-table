#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <random>
#include <cassert>
#include <cstring>
#include <set>
#include <stack>
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
public:
int64_t n, m, k;
double denom;
int64_t string_size;
vector<int64_t> count;
vector<string> keyxor;
vector<int64_t> valuexor;
vector<int64_t> valuesum;

    string str_xor(const string& str1, const string& str2) const {
        string x_xor(string_size, ' ');
        transform(str1.begin(), str1.end(), str2.begin(), x_xor.begin(),
                [](char c1, char c2){ return c1 ^ c2; });
        return x_xor;
    }

    uint64_t take_murmurhash(const string& x_str, int64_t seed) const {
        uint64_t hash_otpt[2];
        const char* x = x_str.data();
        MurmurHash3_x64_128(x, (uint64_t)(x_str.size()), seed, hash_otpt);
        return hash_otpt[1] % m;
    }

    BloomLookupTable(int64_t n, int m, int k, int string_size) : n(n), m(m), k(k), string_size(string_size) {
        count.resize(m, 0);
        denom = (double)m / (double)n;
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuexor.resize(m, 0);
        valuesum.resize(m, 0);
    }

    BloomLookupTable(int64_t n, double denom, int string_size) : n(n), m(denom * n), denom(denom), k(3), string_size(string_size) {
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuexor.resize(m, 0);
        valuesum.resize(m, 0);
    }

    virtual void insert(string x_str, int64_t y) {
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = take_murmurhash(x_str, seed);

            count[k_hash] += 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            valuexor[k_hash] ^= y;
            valuesum[k_hash] += y;
        }
    }

    virtual void remove(string x_str, int64_t y) {
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = take_murmurhash(x_str, seed);

            count[k_hash] -= 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            valuexor[k_hash] ^= y;
            valuesum[k_hash] -= y;
        }
    }

    virtual int get(string x_str) const { // -1 if there is no such key with high probability, -2 if it's 100%
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = take_murmurhash(x_str, seed);

            if (count[k_hash] == 0) {
                return -2;
            }
            if (count[k_hash] == 1 && keyxor[k_hash] == x_str) {
                return valuexor[k_hash];
            }
        }
        return -1;
    }

    Listed list_entries() {
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

    Listed fast_list_entries() {
        Listed result;
        stack<int64_t> ones;

        for (int64_t i = 0; i < count.size(); i++) {
            if (count[i] == 1) {
                ones.push(i);
            }
        }

        while (!ones.empty()) {
            int64_t ind = ones.top();
            ones.pop();

            if (count[ind] != 1) {
                continue;
            }

            result.keys.push_back(keyxor[ind]);
            result.values.push_back(valuexor[ind]);

            string x_str = keyxor[ind];
            int64_t y = valuexor[ind];
            for (uint64_t seed = 0; seed < k; seed++) {
                uint64_t k_hash = take_murmurhash(x_str, seed);

                count[k_hash] -= 1;
                keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
                valuexor[k_hash] ^= y;
                valuesum[k_hash] -= y;

                if (count[k_hash] == 1) {
                    ones.push(k_hash);
                }
            }
        }
        return result;
    }
};


class BloomLookupTableSubstracted : public BloomLookupTable {
public:
    BloomLookupTableSubstracted(int64_t n, int m, int k, int string_size)
        : BloomLookupTable(n, m, k, string_size) {
    }

    vector<int64_t>::iterator find_alice() {
        auto it = count.begin();
        int64_t index = it - count.begin();

        while (it != count.end()) {
            index = it - count.begin();
            if ((count[index] == 1) && (valuesum[index] == valuexor[index])) {
                return it;
            }
            it++;
        }
        return it;
    }

    vector<int64_t>::iterator find_bob() {
        auto it = count.begin();
        int64_t index = it - count.begin();

        while (it != count.end()) {
            index = it - count.begin();
            if ((count[index] == -1) && (-valuesum[index] == valuexor[index])) {
                return it;
            }
            it++;
        }
        return it;
    }

    Entries list_entries() {
        Entries result;
        auto it_alice = find_alice();
        auto it_bob = find_bob();

        while (it_alice != count.end() || it_bob != count.end()) {
            int64_t index_alice = it_alice - count.begin();
            int64_t index_bob = it_bob - count.begin();

            if (it_alice != count.end()) {
                result.Alice.keys.push_back(keyxor[index_alice]);
                result.Alice.values.push_back(valuexor[index_alice]);
                remove(keyxor[index_alice], valuexor[index_alice]);
            }
            if (it_bob != count.end()) {
                result.Bob.keys.push_back(keyxor[index_bob]);
                result.Bob.values.push_back(valuexor[index_bob]);
                insert(keyxor[index_bob], valuexor[index_bob]);
            }
            it_alice = find_alice();
            it_bob = find_bob();
        }
        return result;
    }
 
    Entries fast_list_entries() {
        Entries result;
        stack<int64_t> ones_alice, ones_bob;

        for (int64_t i = 0; i < count.size(); i++) {
            if (count[i] == 1 && (valuesum[i] == valuexor[i])) {
                ones_alice.push(i);
            }
            if (count[i] == -1 && (-valuesum[i] == valuexor[i])) {
                ones_bob.push(i);
            }
        }

        while (!(ones_alice.empty() && ones_bob.emty())) {
            if (!ones_alice.empty()) {
                auto ind_alice = ones_alice.top();
                ones_alice.pop();
                
                if (!(count[ind_alice] == 1 && 
                            (valuesum[ind_alice] == valuexor[ind_alice]))) {
                    continue;
                }

                result.Alice.keys.push_back(keyxor[ind_alice]);
                result.Alice.values.push_back(valuexor[ind_alice]);
                
                string x_str = keyxor[ind_alice];
                int64_t y = valuexor[ind_alice];
                for (uint64_t seed = 0; seed < k; seed++) {
                    uint64_t k_hash = take_murmurhash(x_str, seed);

                    count[k_hash] -= 1;
                    keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
                    valuexor[k_hash] ^= y;
                    valuesum[k_hash] -= y;

                    if (count[k_hash] == 1 && 
                            (valuesum[k_hash] == valuexor[k_hash])) {
                        ones_alice.push(k_hash);
                    }

                    if (count[k_hash] == -1 && 
                            (-valuesum[k_hash] == valuexor[k_hash])) {
                        ones_bob.push(k_hash);
                    }

                }
            }
            if (!ones_bob.empty()) {
                auto ind_bob = ones_bob.top();
                ones_bob.pop();
                
                if (!(count[ind_bob] == -1 && 
                            (-valuesum[ind_bob] == valuexor[ind_bob]))) {
                    continue;
                }

                result.Bob.keys.push_back(keyxor[ind_bob]);
                result.Bob.values.push_back(valuexor[ind_bob]);
                
                string x_str = keyxor[ind_bob];
                int64_t y = valuexor[ind_bob];
                for (uint64_t seed = 0; seed < k; seed++) {
                    uint64_t k_hash = take_murmurhash(x_str, seed);

                    count[k_hash] += 1;
                    keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
                    valuexor[k_hash] ^= y;
                    valuesum[k_hash] += y;

                    if (count[k_hash] == -1 && 
                            (-valuesum[k_hash] == valuexor[k_hash])) {
                        ones_bob.push(k_hash);
                    }

                    if (count[k_hash] == 1 && 
                            (valuesum[k_hash] == valuexor[k_hash])) {
                        ones_alice.push(k_hash);
                    }
                }
            }
        return result;
    }
};

BloomLookupTableSubstracted subtraction(const BloomLookupTable& first, 
        BloomLookupTable& second) {
    auto new_blt = BloomLookupTableSubstracted(first.n, first.m, first.k, 
            first.string_size);
    for (uint64_t i = 0; i < first.m; i++) {
        new_blt.count[i] = first.count[i] - second.count[i];
        new_blt.keyxor[i] = first.str_xor(first.keyxor[i], second.keyxor[i]);
        new_blt.valuexor[i] = first.valuexor[i] ^ second.valuexor[i];
        new_blt.valuesum[i] = first.valuesum[i] - second.valuesum[i];
    }
    return new_blt;
}

