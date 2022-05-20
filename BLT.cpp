#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <random>
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

struct KeyValuePair {
    string key;
    int64_t value = 0;
    bool empty = false;
};

class BloomLookupTable {
public:
int64_t n, m, k; // number of pairs, number of cells, number of hash functions
double denom;
int64_t string_size;
vector<int64_t> count;
vector<string> keyxor;
vector<int64_t> hashvaluesum;
vector<int64_t> valuesum;

    string str_xor(const string& str1_c, const string& str2_c) const { // helper function for xoring strings
        int new_string_size = max(str1_c.size(), str2_c.size());
        string str1(new_string_size - str1_c.size(), ' ');
        string str2(new_string_size - str2_c.size(), ' ');
        str1 += str1_c;
        str2 += str2_c;

        string x_xor(new_string_size, ' ');
        transform(str1.begin(), str1.end(), str2.begin(), x_xor.begin(),
                [](char c1, char c2){ return c1 ^ c2; });
        return x_xor;
    }

    uint64_t take_murmurhash(const string& x_str, int64_t seed) const { // helper function for taking murmurhash of the string
        uint64_t hash_otpt[2];
        const char* x = x_str.data();
        MurmurHash3_x64_128(x, (uint64_t)(x_str.size()), seed, hash_otpt);
        return hash_otpt[1] % m;
    }

    uint64_t take_murmurhash_int64(int64_t value, int64_t seed) const { // helper function for taking murmurhash of int64_t
        uint64_t hash_otpt[2];
        const int64_t* x = &value;
        MurmurHash3_x64_128(x, (uint64_t)sizeof(value), seed, hash_otpt);
        return hash_otpt[1] % m;
    }

    BloomLookupTable(int64_t n, int m, int k, int string_size) : n(n), m(m), k(k), string_size(string_size) {
        count.resize(m, 0);
        denom = (double)m / (double)n;
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        hashvaluesum.resize(m, 0);
        valuesum.resize(m, 0);
    }

    BloomLookupTable(int64_t n, double denom, int string_size) : n(n), m(denom * n), denom(denom), k(3), string_size(string_size) {
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        hashvaluesum.resize(m, 0);
        valuesum.resize(m, 0);
    }

    virtual void insert(string x_str, int64_t y) { // O(k)
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = take_murmurhash(x_str, seed);

            count[k_hash] += 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            hashvaluesum[k_hash] += take_murmurhash_int64(y, 1);
            valuesum[k_hash] += y;
        }
    }

    virtual void remove(string x_str, int64_t y) { // O(k)
        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = take_murmurhash(x_str, seed);

            count[k_hash] -= 1;
            keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
            hashvaluesum[k_hash] -= take_murmurhash_int64(y, 1);
            valuesum[k_hash] -= y;
        }
    }
    
    virtual int get(string x_str) const { // -1 if there is no such key with high probability, -2 if it's 100%
        for (uint64_t seed = 0; seed < k; seed++) { // O(k)
            uint64_t k_hash = take_murmurhash(x_str, seed);

            if (count[k_hash] == 0) {
                return -2;
            }
            if (count[k_hash] == 1 && keyxor[k_hash] == x_str) {
                return valuesum[k_hash];
            }
        }
        return -1;
    }

    Listed list_entries() { // O(n^2)
        Listed result;
        auto it = find(count.begin(), count.end(), 1);
        while (it != count.end()) {
            int64_t index = it - count.begin();
            result.keys.push_back(keyxor[index]);
            result.values.push_back(valuesum[index]);
            remove(keyxor[index], hashvaluesum[index]);
            it = find(count.begin(), count.end(), 1);
        }
        return result;
    }

    Listed fast_list_entries() { // O(n)
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
            result.values.push_back(valuesum[ind]);

            string x_str = keyxor[ind];
            int64_t y = valuesum[ind];
            for (uint64_t seed = 0; seed < k; seed++) {
                uint64_t k_hash = take_murmurhash(x_str, seed);

                count[k_hash] -= 1;
                keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
                hashvaluesum[k_hash] -= take_murmurhash_int64(y, 1);
                valuesum[k_hash] -= y;

                if (count[k_hash] == 1) {
                    ones.push(k_hash);
                }
            }
        }
        return result;
    }
};


class BloomLookupTableSubstracted : public BloomLookupTable { // for Alice and Bob use case
public:
    BloomLookupTableSubstracted(int64_t n, int m, int k, int string_size)
        : BloomLookupTable(n, m, k, string_size) {
    }

    vector<int64_t>::iterator find_alice() { // helper function
        auto it = count.begin();
        int64_t index = it - count.begin();

        while (it != count.end()) {
            index = it - count.begin();
            if ((count[index] == 1) &&
            (take_murmurhash_int64(valuesum[index], 1) == hashvaluesum[index])) {
                return it;
            }
            it++;
        }
        return it;
    }

    vector<int64_t>::iterator find_bob() { // helper function
        auto it = count.begin();
        int64_t index = it - count.begin();

        while (it != count.end()) {
            index = it - count.begin();
            if ((count[index] == -1) &&
            (take_murmurhash_int64(-valuesum[index], 1) == -hashvaluesum[index])) {
                return it;
            }
            it++;
        }
        return it;
    }

    Entries list_entries() { // O(n^2)
        Entries result;
        auto it_alice = find_alice();
        auto it_bob = find_bob();

        while (it_alice != count.end() || it_bob != count.end()) {
            int64_t index_alice = it_alice - count.begin();
            int64_t index_bob = it_bob - count.begin();

            if (it_alice != count.end()) {
                result.Alice.keys.push_back(keyxor[index_alice]);
                result.Alice.values.push_back(valuesum[index_alice]);
                remove(keyxor[index_alice], valuesum[index_alice]);
            }
            if (it_bob != count.end()) {
                result.Bob.keys.push_back(keyxor[index_bob]);
                result.Bob.values.push_back(-valuesum[index_bob]);
                insert(keyxor[index_bob], -valuesum[index_bob]);
            }
            it_alice = find_alice();
            it_bob = find_bob();
        }
        return result;
    }
 
    Entries fast_list_entries() { // O(n)
        Entries result;
        stack<int64_t> ones_alice, ones_bob;
        int stack_size = 0;

        for (int64_t i = 0; i < count.size(); i++) {
            if (count[i] == 1 &&
            (take_murmurhash_int64(valuesum[i], 1) == hashvaluesum[i])) {
                ones_alice.push(i); stack_size++;
            }
            if (count[i] == -1 &&
            (take_murmurhash_int64(-valuesum[i], 1) == -hashvaluesum[i])) {
                ones_bob.push(i); stack_size++;
            }
        }

        while (!(ones_alice.empty() && ones_bob.empty())) {
            if (!ones_alice.empty()) {
                auto ind_alice = ones_alice.top();
                ones_alice.pop();
                
                if (!(count[ind_alice] == 1 &&
                        (take_murmurhash_int64(valuesum[ind_alice], 1) == hashvaluesum[ind_alice]))) {
                    continue;
                }

                result.Alice.keys.push_back(keyxor[ind_alice]);
                result.Alice.values.push_back(valuesum[ind_alice]);
                
                string x_str = keyxor[ind_alice];
                int64_t y = valuesum[ind_alice];
                for (uint64_t seed = 0; seed < k; seed++) {
                    uint64_t k_hash = take_murmurhash(x_str, seed);

                    count[k_hash] -= 1;
                    keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
                    hashvaluesum[k_hash] -= take_murmurhash_int64(y, 1);
                    valuesum[k_hash] -= y;

                    if (count[k_hash] == 1 &&
                            (take_murmurhash_int64(valuesum[k_hash], 1) == hashvaluesum[k_hash])) {
                        ones_alice.push(k_hash); stack_size++;
                    }

                    if (count[k_hash] == -1 &&
                            (take_murmurhash_int64(-valuesum[k_hash], 1) == -hashvaluesum[k_hash])) {
                        ones_bob.push(k_hash); stack_size++;
                    }

                }
            }
            if (!ones_bob.empty()) {
                auto ind_bob = ones_bob.top();
                ones_bob.pop();
                
                if (!(count[ind_bob] == -1 &&
                        (take_murmurhash_int64(-valuesum[ind_bob], 1) == -hashvaluesum[ind_bob]))) {
                    continue;
                }

                result.Bob.keys.push_back(keyxor[ind_bob]);
                result.Bob.values.push_back(-valuesum[ind_bob]);
                
                string x_str = keyxor[ind_bob];
                int64_t y = -valuesum[ind_bob];
                for (uint64_t seed = 0; seed < k; seed++) {
                    uint64_t k_hash = take_murmurhash(x_str, seed);

                    count[k_hash] += 1;
                    keyxor[k_hash] = str_xor(keyxor[k_hash], x_str);
                    hashvaluesum[k_hash] += take_murmurhash_int64(y, 1);
                    valuesum[k_hash] += y;

                    if (count[k_hash] == -1 &&
                            (take_murmurhash_int64(-valuesum[k_hash], 1) == -hashvaluesum[k_hash])) {
                        ones_bob.push(k_hash); stack_size++;
                    }

                    if (count[k_hash] == 1 &&
                            (take_murmurhash_int64(valuesum[k_hash], 1) == hashvaluesum[k_hash])) {
                        ones_alice.push(k_hash); stack_size++;
                    }
                }
            }
            if (stack_size > 4 * m) {
                break;
            }
        }
        return result;
    }

    KeyValuePair unpoison(KeyValuePair& a) { // O(k)
        KeyValuePair result;
        result.key = a.key;
        result.empty = true;

        for (uint64_t seed = 0; seed < k; seed++) {
            uint64_t k_hash = take_murmurhash(a.key, seed);
            int64_t vsum = valuesum[k_hash] + a.value;
            int64_t hofvsum = take_murmurhash_int64(valuesum[k_hash] + a.value, 1);
            int64_t sumhash = (hashvaluesum[k_hash] + take_murmurhash_int64(a.value, 1));

            if ((count[k_hash] + 1 == 1) &&
            ((hashvaluesum[k_hash] + take_murmurhash_int64(a.value, 1)) ==
            take_murmurhash_int64(valuesum[k_hash] + a.value, 1))) {
                // cout << "unpoison" << endl;
                result.value = valuesum[k_hash] + a.value;
                result.empty = false;
                insert(a.key, a.value);
                remove(result.key, result.value);
                break;
            }
        }
        return result;
    }

    Entries poisoned_list_entries(vector<KeyValuePair>& bob_pairs) { // O(n * m)
        Entries result = fast_list_entries();

        vector<bool> bob_left(bob_pairs.size(), true);
        int left = bob_left.size();

        for (int i = 0; i < bob_pairs.size(); i++) {
            if (find(result.Bob.keys.begin(), result.Bob.keys.end(), bob_pairs[i].key) != result.Bob.keys.end()) {
                bob_left[i] = false;
                left--;
            }
        }
        
        int u = 0;
        int j = 0;
        while (left > 0) {
            j++;
            bool any_unpoisoned = false;
            for (int i = 0; i < bob_pairs.size(); i++) {
                auto bob_pair = bob_pairs[i];
                if (!bob_left[i]) {
                    continue;
                }
                auto alice_pair = unpoison(bob_pair);
                if (!alice_pair.empty) {
                    u++;
                    any_unpoisoned = true;
                    left--;
                    bob_left[i] = false;
                    result.Alice.keys.push_back(alice_pair.key);
                    result.Alice.values.push_back(alice_pair.value);

                    result.Bob.keys.push_back(bob_pair.key);
                    result.Bob.values.push_back(bob_pair.value);
                }
            }
            if (!any_unpoisoned) {
                break;
            } else {
                Entries result_update = fast_list_entries();

                for (int i = 0; i < bob_pairs.size(); i++) {
                    if (find(result_update.Bob.keys.begin(), result_update.Bob.keys.end(),
                             bob_pairs[i].key) != result_update.Bob.keys.end()) {
                        bob_left[i] = false;
                        left--;
                    }
                }

                for (int i = 0; i < result_update.Alice.keys.size(); i++) {
                    result.Alice.keys.push_back(result_update.Alice.keys[i]);
                    result.Alice.values.push_back(result_update.Alice.values[i]);
                }

                for (int i = 0; i < result_update.Bob.keys.size(); i++) {
                    result.Bob.keys.push_back(result_update.Bob.keys[i]);
                    result.Bob.values.push_back(result_update.Bob.values[i]);
                }
            }
        }
        return result;
    }
};

BloomLookupTableSubstracted subtraction(const BloomLookupTable& first, // for Alice and Bob use case
        BloomLookupTable& second) {
    auto new_blt = BloomLookupTableSubstracted(first.n, first.m, first.k, 
            first.string_size);
    for (uint64_t i = 0; i < first.m; i++) {
        new_blt.count[i] = first.count[i] - second.count[i];
        new_blt.keyxor[i] = first.str_xor(first.keyxor[i], second.keyxor[i]);
        new_blt.hashvaluesum[i] = first.hashvaluesum[i] - second.hashvaluesum[i];
        new_blt.valuesum[i] = first.valuesum[i] - second.valuesum[i];
    }
    return new_blt;
}
