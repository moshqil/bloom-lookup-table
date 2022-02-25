#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <random>
#include <cassert>

using namespace std;

class BloomLookupTable {
private:
    int n, m, k;
    int string_size;
    vector<int> count;
    vector<string> keyxor;
    vector<int> valuesum;

public:
    BloomLookupTable(int n, int m, int k, int string_size) : n(n), m(m), k(k), string_size(string_size) {
        count.resize(m, 0);
        string empty_string(string_size, 0);
        keyxor.resize(m, empty_string);
        valuesum.resize(m, 0);
    }
    void insert(string x, int y) {
        for (char i = '!'; i < (char)('!' + k); i++) {
            int k_hash = hash<string>{}(x + i) % m;

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
            int k_hash = hash<string>{}(x + i) % m;

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
            int k_hash = hash<string>{}(x + i) % m;
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

    void stress_test_better_random(int number_of_keys, int iteration_number) {
        random_device rd;
        mt19937 gen32(iteration_number);

        string alphanum =
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        vector<string> keys_rand;
        vector<int> values_rand;
        for (int i = 0; i < number_of_keys; i++) {
            string s;
            for (int j = 0; j < string_size; j++) {
                s += alphanum[gen32() % alphanum.size()];
            }
            int v = gen32();
            insert(s, v);
            keys_rand.push_back(s);
            values_rand.push_back(v);
        }
        for (int i = 0; i < keys_rand.size(); i++) {
            if (get(keys_rand[i]) != values_rand[i]) {
                cout << "mistake" << endl;
                return;
            }
        }
        cout << "ok" << endl;
    }

    void stress_test_list_entries(int number_of_keys, int iteration_number) {
        random_device rd;
        mt19937 gen32(iteration_number);

        string alphanum =
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        vector<string> keys_rand;
        vector<int> values_rand;
        for (int i = 0; i < number_of_keys; i++) {
            string s;
            for (int j = 0; j < string_size; j++) {
                s += alphanum[gen32() % alphanum.size()];
            }
            int v = gen32();
            insert(s, v);
            keys_rand.push_back(s);
            values_rand.push_back(v);
        }
        vector<string> keys_output;
        vector<int> values_output;
        list_entries(keys_output, values_output);
        for (int i = 0; i < keys_rand.size(); i++) {
            if (find(keys_output.begin(), keys_output.end(), keys_rand[i]) == keys_output.end()) {
                cout << "mistake" << endl;
                return;
            }
        }
        cout << "ok" << endl;
    }

};

int main() {
    // auto B = BloomLookupTable(20, 100, 3, 3);
    // B.insert("abc", 179);
    // B.insert("bcd", 23);
    // B.insert("mas", 12);
    // B.insert("wtf", 57);

    // vector<string> ok; vector<int> ov;

    // cout << B.get("abc") << endl;
    // cout << B.get("bcd") << endl;

    //vector<string> ok; vector<int> ov;
    // B.remove("mas", 12);
    // B.list_entries(ok, ov);
    // for (auto& elem : ok) {
        // cout << elem << endl;
    // }
    // cout << '!' << endl;
    // for (auto& elem : ov) {
        // cout << elem << endl;
    // }
    // cout << endl;

    time_t time_now = time(nullptr) % 1000; // for better random
    
    cout << "List entries (10 tests with 100 pairs and filter of size 150):" << endl;
    for (int i = 0; i < 10; i++) {
        auto B1 = BloomLookupTable(100, 150, 3, 10);
        B1.stress_test_list_entries(100, time_now * (i + 1));
    }


    cout << "Insert and get (10 tests with 100 pairs and filter of size 1500):" << endl;
    for (int i = 10; i < 20; i++) {
        auto B1 = BloomLookupTable(100, 1500, 10, 10);
        B1.stress_test_better_random(100, time_now * (i + 1));
    }

}
