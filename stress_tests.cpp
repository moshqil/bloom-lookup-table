#include "BLT.cpp"


vector<string> generate_string_vector(int64_t n, 
        int64_t string_size, int seed) {
    random_device rd;
    mt19937 gen32(seed);
    string alphanum = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    vector<string> string_rand;
    for (int64_t i = 0; i < n; i++) {
        string s;
        for (int64_t j = 0; j < string_size; j++) {
            s += alphanum[gen32() % alphanum.size()];
        }
        string_rand.push_back(s);
    }
    return string_rand;
}

vector<int64_t> generate_number_vector(int64_t n, int seed) {
    random_device rd;
    mt19937 gen32(seed);
    vector<int64_t> number_rand;
    for (int64_t i = 0; i < n; i++) {
        number_rand.push_back(gen32());
    }
    return number_rand;
}


double stress_test_get(int it_number, int n, int m, int k, 
        int string_size, int seed) {
    vector<string> keys;
    vector<int64_t> values;

    double successes = (double)it_number;

    for (int i = 0; i < it_number; i++) {
        auto BLT = BloomLookupTable(n, m, k, string_size);
        keys = generate_string_vector(n, string_size, seed + i);
        values = generate_number_vector(n, seed + i);

        for (int j = 0; j < n; j++) {
            BLT.insert(keys[j], values[j]);
        }

        for (int j = 0; j < n; j++) {
            if (BLT.get(keys[j]) != values[j]) {
                successes--;
                break;
            }
        }
    }
    return 100.0 * (successes / (double)it_number);
}

double stress_test_list_entries(int it_number, int n, int m, int k, 
        int string_size, int seed) {
    vector<string> keys;
    vector<int64_t> values;

    double successes = (double)it_number;

    for (int i = 0; i < it_number; i++) {
        auto BLT = BloomLookupTable(n, m, k, string_size);
        keys = generate_string_vector(n, string_size, seed + i);
        values = generate_number_vector(n, seed + i);

        for (int j = 0; j < n; j++) {
            BLT.insert(keys[j], values[j]);
        }

        Listed result = BLT.list_entries();
        if (result.keys.size() != keys.size() || 
                result.values.size() != values.size()) {
            successes--;
            continue;
        }
        for (int j = 0; j < n; j++) {
            if (find(keys.begin(), keys.end(), result.keys[j]) == keys.end() || 
                   find(values.begin(), values.end(), result.values[j])
                   == values.end()) {
                successes--;
                break;
            }
        }
    }
    return 100.0 * (successes / (double)it_number);
}



