#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

class BloomLookupTable {
private:
    int n; int m; int k;
    int string_size;
    vector<int> count;
    vector<string> keyxor;
    vector<int> valuesum;

public:
    BloomLookupTable(int n, int k, int m, int string_size) : n(n), m(m), string_size(string_size) {
        k = 0.6931 * m / n;
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuesum.resize(m, 0);
    }
    void insert(string x, int y) {
        for (char i = '!'; i < (char)('!' + k); i++) {
            int k_hash = hash<string>{}(x + i);

            string x_xor(x.size(), ' ');
            transform(x.begin(), x.end(), keyxor[k_hash].begin(), x_xor.begin(),
                      [](char c1, char c2){ return c1 ^ c2; });

            count[k_hash] += 1;
            keyxor[k_hash] = x_xor;
            valuesum[k_hash] += y;
        }
    }

    void remove(string x, int y) {
        for (char i = '!'; i < (char)('!' + k); i++) {
            int k_hash = hash<string>{}(x + i);

            string x_xor(x.size(), ' ');
            transform(x.begin(), x.end(), keyxor[k_hash].begin(), x_xor.begin(),
                      [](char c1, char c2){ return c1 ^ c2; });

            count[k_hash] -= 1;
            keyxor[k_hash] = x_xor;
            valuesum[k_hash] -= y;
        }
    }

    int get(string x) { // -1 if there is no such key with high probability, -2 if it's 100%
        for (char i = '!'; i < (char)('!' + k); i++) {
            int k_hash = hash<string>{}(x + i);
            string empty_string(string_size, 0);
            if (count[k_hash] == 0 && keyxor[k_hash] == empty_string) {
                return -2;
            }
            if (count[k_hash] == 1 && keyxor[k_hash] == x) {
                return valuesum[k_hash];
            }
        }
        return -1;
    }

    void list_entries(vector<string>& output_keys, vector<int>& output_values) {
        auto it = find(count.begin(), count.end(), 1);
        while (it != count.end()) {
            int index = it - count.begin();
            output_keys.push_back(keyxor[index]);
            output_values.push_back(valuesum[index]);
            remove(keyxor[index], valuesum[index]);
            it = find(count.begin(), count.end(), 1);
        }
    }

};
