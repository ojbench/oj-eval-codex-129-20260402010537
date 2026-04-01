#include <bits/stdc++.h>
using namespace std;

struct Value {
    bool is_int; // true for int, false for string
    long long iv; // will fit in int per problem, keep long long safe
    string sv;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    // scopes: each scope has unordered_map name -> Value
    vector<unordered_map<string, Value>> scopes;
    scopes.emplace_back(); // global scope

    auto find_var = [&](const string &name) -> pair<int, Value*> {
        for (int i = (int)scopes.size() - 1; i >= 0; --i) {
            auto it = scopes[i].find(name);
            if (it != scopes[i].end()) return {i, &it->second};
        }
        return {-1, (Value*)nullptr};
    };

    string op;
    string tmp;
    for (int i = 0; i < n; ++i) {
        if (!(cin >> op)) break;
        bool valid = true;

        if (op == "Indent") {
            scopes.emplace_back();
            // no output
            continue;
        } else if (op == "Dedent") {
            if (scopes.size() > 1) {
                scopes.pop_back();
            } else {
                // cannot dedent beyond global
                valid = false;
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else if (op == "Declare") {
            string type, name;
            if (!(cin >> type >> name)) { valid = false; }
            if (!valid) {
                cout << "Invalid operation\n"; 
                // try to recover by discarding the rest of line value token
                // but input guarantees parameter count, so skip
                continue;
            }
            if (type == "int") {
                long long v; 
                if (!(cin >> v)) { valid = false; }
                else {
                    // can declare shadowing in same scope? In C++ cannot redeclare, but problem states if name not defined in current scope, then can define. It does not clarify error on redeclare; we treat redeclare in same scope as invalid.
                    auto &cur = scopes.back();
                    if (cur.find(name) != cur.end()) valid = false;
                    else cur[name] = Value{true, v, string()};
                }
            } else if (type == "string") {
                // read a quoted string token possibly with spaces? The problem states value is a string constant in quotes without escapes and input tokens are space-separated; sample shows strings without spaces like "SJTU" or with spaces? Public data shows many strings like " profile" with leading space inside quotes.
                // But since input uses whitespace separators, strings will be a single token with embedded spaces? Not possible. Observed public data shows tokens like " profile" including space inside quotes: but read via >> would read as " since spaces split tokens. However, they include space immediately after opening quote? Actually token is " profile" (with space after opening quote) still a single token since includes no spaces outside quotes in file. The space is inside the quotes but the token is contiguous including quotes and preceding spaces are part of token. Using operator>> will read the whole token including quotes.
                string tok; 
                if (!(cin >> tok)) { valid = false; }
                else {
                    // token starts with '"' and ends with '"'
                    if (tok.size() >= 2 && tok.front() == '"' && tok.back() == '"') {
                        string content = tok.substr(1, tok.size() - 2);
                        auto &cur = scopes.back();
                        if (cur.find(name) != cur.end()) valid = false;
                        else cur[name] = Value{false, 0, content};
                    } else {
                        valid = false;
                    }
                }
            } else {
                valid = false;
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else if (op == "Add") {
            string res, v1, v2;
            if (!(cin >> res >> v1 >> v2)) { valid = false; }
            if (valid) {
                auto pr = find_var(res);
                auto p1 = find_var(v1);
                auto p2 = find_var(v2);
                if (pr.second == nullptr || p1.second == nullptr || p2.second == nullptr) valid = false;
                else if (pr.second->is_int != p1.second->is_int || pr.second->is_int != p2.second->is_int) valid = false;
                else {
                    if (pr.second->is_int) {
                        pr.second->iv = p1.second->iv + p2.second->iv;
                    } else {
                        pr.second->sv = p1.second->sv + p2.second->sv;
                    }
                }
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else if (op == "SelfAdd") {
            string name;
            if (!(cin >> name)) { valid = false; }
            if (valid) {
                auto pv = find_var(name);
                if (pv.second == nullptr) valid = false;
                else if (pv.second->is_int) {
                    long long inc;
                    if (!(cin >> inc)) valid = false;
                    else pv.second->iv += inc;
                } else {
                    string tok;
                    if (!(cin >> tok)) valid = false;
                    else if (tok.size() >= 2 && tok.front() == '"' && tok.back() == '"') {
                        pv.second->sv += tok.substr(1, tok.size() - 2);
                    } else valid = false;
                }
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else if (op == "Print") {
            string name;
            if (!(cin >> name)) { valid = false; }
            if (valid) {
                auto pv = find_var(name);
                if (pv.second == nullptr) valid = false;
                else {
                    if (pv.second->is_int) cout << name << ':' << (long long)pv.second->iv << '\n';
                    else cout << name << ':' << pv.second->sv << '\n';
                }
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else {
            // unknown op (shouldn't happen per problem)
            // consume rest of line cautiously
            valid = false;
            cout << "Invalid operation\n";
            continue;
        }
    }
    return 0;
}

