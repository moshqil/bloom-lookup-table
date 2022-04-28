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
}

struct Entries {
    Listed Alice;
    Listed Bob;
}

class BloomLookupTable {
private:
    int64_t n, m, k;
    int64_t string_size;
    vector<int64_t> count;
    vector<string> keyxor;
    vector<int64_t> valuesum;
    bool is_modified = false;

public:
    BloomLookupTable(int64_t n, int m, int k, int string_size) : n(n), m(m), k(k), string_size(string_size) {
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuesum.resize(m, 0);
    }

    BloomLookupTable(int64_t n, double denom, int string_size) : n(n), m(denom * n), k(3), string_size(string_size) {
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuesum.resize(m, 0);
    }

    BloomLookupTable& operator-=(const BloomLookupTable& other) {
        is_modified = true;
        for (uint64_t index = 0; index < m; index++) {
            count[index] -= other.count[index];
            
            string x_xor(x_str.size(), ' ');
            transform(other.keyxor[index].begin(), other.keyxor[index].end(), keyxor[index].begin(), x_xor.begin(),
                      [](char c1, char c2){ return c1 ^ c2; });
            keyxor[index] = x_xor;

            valuesum[index] -= other.valuesum[index];
        }
        return *this;
    }

    void insert(string x_str, int64_t y) {
        const char* x = x_str.data();
        uint64_t hash_otpt[2];
        for (uint64_t seed = 0; seed < k; seed++) {
            // int64_t k_hash = hash<string>{}(x + i) % m;
           
            string x_xor(x_str.size(), ' ');
            MurmurHash3_x64_128(x, (uint64_t)(x_str.size()), seed, hash_otpt);
            uint64_t k_hash = hash_otpt[1] % m;


            transform(x_str.begin(), x_str.end(), keyxor[k_hash].begin(), x_xor.begin(),
                      [](char c1, char c2){ return c1 ^ c2; });

            count[k_hash] += 1;
            keyxor[k_hash] = x_xor;
            valuesum[k_hash] += y;
        }
    }

    void remove(string x_str, int64_t y) {
        const char* x = x_str.data();
        uint64_t hash_otpt[2];
        for (uint64_t seed = 0; seed < k; seed++) {
            string x_xor(x_str.size(), ' ');
            MurmurHash3_x64_128(x, (uint64_t)(x_str.size()), seed, hash_otpt);
            uint64_t k_hash = hash_otpt[1] % m;

            transform(x_str.begin(), x_str.end(), keyxor[k_hash].begin(), x_xor.begin(),
                      [](char c1, char c2){ return c1 ^ c2; });

            count[k_hash] -= 1;
            keyxor[k_hash] = x_xor;
            valuesum[k_hash] -= y;
        }
    }

    int64_t get(string x_str) { // -1 if there is no such key with high probability, -2 if it's 100%
        const char* x = x_str.data();
        uint64_t hash_otpt[2];
        for (uint64_t seed = 0; seed < k; seed++) {
            string x_xor(x_str.size(), ' ');
            MurmurHash3_x64_128(x, (uint64_t)(x_str.size()), seed, hash_otpt);
            uint64_t k_hash = hash_otpt[1] % m;

            string empty_string(string_size, 0);

            if (count[k_hash] == 0 && (keyxor[k_hash] == empty_string)) {
                return -2;
            }
            if (count[k_hash] == 1 && keyxor[k_hash] == x) {
                return valuesum[k_hash];
            }
        }
        return -1;
    }

    Entries list_entries() {
        Entries result;
        auto it_alice = find(count.begin(), count.end(), 1);
        auto it_bob = find(count.begin(), count.end(), -1);
        while (it_alice != count.end() && it_bob != count.end()) {
            int64_t index_alice = it_alice - count.begin();
            int64_t index_bob = it_bob - count.begin();
            if (it_alice != count.end()) {
                result.Alice.keys.push_back(keyxor[index_alice]);
                result.Alice.values.push_back(valuesum[index_alice]);
                remove(keyxor[index_alice], valuesum[index_alice]);
            }
            if (it_bob != count.end()) {
                result.Bob.keys.push_back(keyxor[index_bob]);
                result.Bob.values.push_back(valuesum[index_bob]);
                insert(keyxor[index_bob], valuesum[index_bob]);
            }
            it_alice = find(count.begin(), count.end(), 1);
            it_bob = find(count.begin(), count.end(), -1);
        }
    }

    void stress_test_better_random(int64_t number_of_keys, int iteration_number) {
        random_device rd;
        mt19937 gen32(iteration_number);

        string alphanum =
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        vector<string> keys_rand;
        vector<int64_t> values_rand;
        for (int64_t i = 0; i < number_of_keys; i++) {
            string s;
            for (int64_t j = 0; j < string_size; j++) {
                s += alphanum[gen32() % alphanum.size()];
            }
            int64_t v = gen32();
            insert(s, v);
            keys_rand.push_back(s);
            values_rand.push_back(v);
        }
        for (int64_t i = 0; i < keys_rand.size(); i++) {
            if (get(keys_rand[i]) != values_rand[i]) {
                cout << "mistake" << endl;
                return;
            }
        }
        cout << "ok" << endl;
    }

    bool stress_test_list_entries(int64_t number_of_keys, int iteration_number) {
        random_device rd;
        mt19937 gen32(iteration_number);

        string alphanum =
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        vector<string> keys_rand;
        vector<int64_t> values_rand;
        for (int64_t i = 0; i < number_of_keys; i++) {
            string s;
            for (int64_t j = 0; j < string_size; j++) {
                s += alphanum[gen32() % alphanum.size()];
            }
            int64_t v = gen32();
            insert(s, v);
            keys_rand.push_back(s);
            values_rand.push_back(v);
        }
        vector<string> keys_output;
        vector<int64_t> values_output;
        list_entries(keys_output, values_output);
        for (int64_t i = 0; i < keys_rand.size(); i++) {
            if (find(keys_output.begin(), keys_output.end(), keys_rand[i]) == keys_output.end()) {
                return false;
            }
        }
        return true;
    }

};

int main() {    

    time_t time_now = time(nullptr) % 1000; // for better random

    cout << "Number of TESTS for list entries method stress test:\n";
    int tests1; cin >> tests1;
    cout << "Number of PAIRS for list entries method stress test:\n ";
    int pairs1; cin >> pairs1;
    cout << "List entries (" << tests1 << " tests with " << pairs1 << " pairs):" << endl;
    vector<double> denoms = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2};

    for (double denom : denoms) {
        int count_ok = 0;
        for (int64_t i = 0; i < tests1; i++) {
            auto B1 = BloomLookupTable(pairs1, denom, 5);
            count_ok += B1.stress_test_list_entries(pairs1, time_now * (i + 1));
        }
        cout << "Percentege of successes for ratio " << denom << " is " << count_ok / 10 << "." << count_ok % 10 << endl;
    }
}
