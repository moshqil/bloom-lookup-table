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

        Listed result = BLT.fast_list_entries();
        if (result.keys.size() != keys.size() || 
                result.values.size() != values.size()) {
            successes--;
        }

        // sort(result.keys.begin(), result.keys.end());
        // sort(keys.begin(), keys.end());
        
        // if (keys != result.keys) {
        //    successes--;
        //}

    }
    return 100.0 * (successes / (double)it_number);
}

double stress_test_list_entries_subtracted_empty_intersection(int it_number, int n_alice, int n_bob, int m, int k,
        int string_size, int seed) {

    vector<string> keys1, keys2;
    vector<int64_t> values1, values2;

    double successes = (double)it_number;

    for (int i = 0; i < it_number; i++) {
        auto BLT1 = BloomLookupTable(n_alice, m, k, string_size);
        auto BLT2 = BloomLookupTable(n_bob, m, k, string_size);

        keys1 = generate_string_vector(n_alice, string_size, 2 * (seed + i));
        keys2 = generate_string_vector(n_bob, string_size, 2 * (seed + i) + 1);

        values1 = generate_number_vector(n_alice, 2 * (seed + i));
        values2 = generate_number_vector(n_bob, 2 * (seed + i) + 1);

        for (int j = 0; j < n_alice; j++) {
            BLT1.insert(keys1[j], values1[j]);
        }
        for (int j = 0; j < n_bob; j++) {
            BLT2.insert(keys2[j], values2[j]);
        }

        auto sub_BLT = subtraction(BLT1, BLT2);

        Entries result = sub_BLT.fast_list_entries();

        if (result.Alice.keys.size() != keys1.size() || 
                result.Bob.keys.size() != keys2.size()) {
            successes--;
        }
    }
    return 100.0 * (successes / (double)it_number);
}
