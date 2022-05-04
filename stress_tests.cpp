#include <BLT.cpp>



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
    Entries result = list_entries();
    for (int64_t i = 0; i < keys_rand.size(); i++) {
        if (find(result.Alice.keys.begin(), result.Alice.keys.end(), keys_rand[i]) == result.Alice.keys.end()) {
            return false;
        }
    }
    return true;
}

bool stress_test_list_entries_subtraction(int64_t number_of_keys, int iteration_number) {
    random_device rd;
    mt19937 gen32(iteration_number);

    string alphanum =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    vector<string> keys_rand_alice;
    vector<int64_t> values_rand_alice;
    for (int64_t i = 0; i < number_of_keys / 2; i++) {
        string s_alice;
        for (int64_t j = 0; j < string_size; j++) {
            s_alice += alphanum[gen32() % alphanum.size()];
        }
        int64_t v_alice = gen32();
        insert(s_alice, v_alice);
        keys_rand_alice.push_back(s_alice);
        values_rand_alice.push_back(v_alice);
        cout << "alice's pair is " << s_alice << " " << v_alice << endl;
    }

    BloomLookupTable bob(n, denom, string_size);
    vector<string> keys_rand_bob;
    vector<int64_t> values_rand_bob;
    for (int64_t i = 0; i < number_of_keys / 2; i++) {
        string s_bob;
        for (int64_t j = 0; j < string_size; j++) {
            s_bob += alphanum[gen32() % alphanum.size()];
        }
        int64_t v_bob = gen32();
        bob.insert(s_bob, v_bob);
        keys_rand_bob.push_back(s_bob);
        values_rand_bob.push_back(v_bob);
        cout << "bob's pair is " << s_bob << " " << v_bob << endl;
    }

    *this -= bob;
    Entries result = list_entries();
    for (int64_t i = 0; i < keys_rand_alice.size(); i++) {
        cout << i << endl;
        if (find(result.Alice.keys.begin(), result.Alice.keys.end(), keys_rand_alice[i]) == result.Alice.keys.end()) {
            return false;
        }
    }
    for (int64_t i = 0; i < keys_rand_bob.size(); i++) {
        if (find(result.Bob.keys.begin(), result.Bob.keys.end(), keys_rand_bob[i]) == result.Bob.keys.end()) {
            return false;
        }
    }

    return true;
}
