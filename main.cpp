#include <bits/stdc++.h>
using namespace std;

struct Value {
    bool is_int; // true for int, false for string
    long long iv; // will fit in int per problem, keep long long safe
    string sv;
};

static inline bool is_valid_name(const string &s) {
    if (s.empty()) return false;
    auto is_alpha = [](char c){ return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||c=='_'; };
    auto is_alnum = [&](char c){ return is_alpha(c) || (c>='0'&&c<='9'); };
    if (!is_alpha(s[0])) return false;
    for (size_t i = 1; i < s.size(); ++i) if (!is_alnum(s[i])) return false;
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    // Use a global symbol table: name -> stack of values for scoping;
    // And a stack of sets tracking which names were declared in each scope level
    vector<unordered_set<string>> scope_declared;
    scope_declared.emplace_back(); // global scope
    unordered_map<string, vector<Value>> table; // name -> stack of Values

    string op;
    string tmp;
    for (int i = 0; i < n; ++i) {
        if (!(cin >> op)) break;
        bool valid = true;

        if (op == "Indent") {
            scope_declared.emplace_back();
            // no output
            continue;
        } else if (op == "Dedent") {
            if (scope_declared.size() > 1) {
                // pop all names declared in this scope from table
                for (const auto &name : scope_declared.back()) {
                    auto it = table.find(name);
                    if (it != table.end() && !it->second.empty()) {
                        it->second.pop_back();
                        if (it->second.empty()) table.erase(it);
                    }
                }
                scope_declared.pop_back();
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
            if (!is_valid_name(name)) {
                // need still to consume the value token(s)
                if (type == "int") {
                    long long dummy; cin >> dummy; // consume
                } else if (type == "string") {
                    string tok; if (cin >> tok) {
                        if (!tok.empty() && tok.front() == '"' && !(tok.size()>=2 && tok.back()=='"')) {
                            string more; while (cin >> more) { if (!more.empty() && more.back()=='"') break; }
                        }
                    }
                }
                cout << "Invalid operation\n";
                continue;
            }
            if (type == "int") {
                long long v; 
                if (!(cin >> v)) { valid = false; }
                else {
                    // cannot redeclare in the same scope
                    auto &curset = scope_declared.back();
                    if (curset.find(name) != curset.end()) valid = false;
                    else {
                        curset.insert(name);
                        table[name].push_back(Value{true, v, string()});
                    }
                }
            } else if (type == "string") {
                // robustly read a quoted string, which may include spaces but has no escapes
                // We get the next token; if it starts with '"' and ends without '"', keep reading and appending with spaces until closing quote
                string tok;
                if (!(cin >> tok)) { valid = false; }
                else {
                    if (tok.empty() || tok.front() != '"') { valid = false; }
                    else {
                        string content;
                        if (tok.size() >= 2 && tok.back() == '"') {
                            content = tok.substr(1, tok.size() - 2);
                        } else {
                            content = tok.substr(1);
                            string more;
                            bool closed = false;
                            while (cin >> more) {
                                if (!more.empty() && more.back() == '"') {
                                    // closing
                                    content += ' ' + more.substr(0, more.size() - 1);
                                    closed = true;
                                    break;
                                } else {
                                    content += ' ' + more;
                                }
                            }
                            if (!closed) valid = false;
                        }
                        if (valid) {
                            auto &curset = scope_declared.back();
                            if (curset.find(name) != curset.end()) valid = false;
                            else {
                                curset.insert(name);
                                table[name].push_back(Value{false, 0, content});
                            }
                        }
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
                auto itR = table.find(res);
                auto it1 = table.find(v1);
                auto it2 = table.find(v2);
                if (itR == table.end() || itR->second.empty() || it1 == table.end() || it1->second.empty() || it2 == table.end() || it2->second.empty()) valid = false;
                else if (itR->second.back().is_int != it1->second.back().is_int || itR->second.back().is_int != it2->second.back().is_int) valid = false;
                else {
                    if (itR->second.back().is_int) {
                        itR->second.back().iv = it1->second.back().iv + it2->second.back().iv;
                    } else {
                        itR->second.back().sv = it1->second.back().sv + it2->second.back().sv;
                    }
                }
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else if (op == "SelfAdd") {
            string name;
            if (!(cin >> name)) { valid = false; }
            // Regardless of validity, we must consume the value argument(s)
            string firstTok;
            if (!(cin >> firstTok)) {
                cout << "Invalid operation\n";
                continue;
            }
            auto it = table.find(name);
            bool hasVar = (it != table.end() && !it->second.empty());
            if (!hasVar) {
                // consume generically
                if (!firstTok.empty() && firstTok.front() == '"' && !(firstTok.size()>=2 && firstTok.back()=='"')) {
                    string more; while (cin >> more) { if (!more.empty() && more.back()=='"') break; }
                }
                cout << "Invalid operation\n";
                continue;
            }
            if (it->second.back().is_int) {
                // firstTok should be an integer; if it's a quoted string, still consume rest and mark invalid
                if (!firstTok.empty() && firstTok.front()=='"') {
                    if (!(firstTok.size()>=2 && firstTok.back()=='"')) { string more; while (cin >> more) { if (!more.empty() && more.back()=='"') break; } }
                    cout << "Invalid operation\n";
                    continue;
                }
                // parse integer
                bool oknum = true;
                try { long long inc = stoll(firstTok); it->second.back().iv += inc; }
                catch (...) { oknum = false; }
                if (!oknum) cout << "Invalid operation\n";
            } else {
                if (firstTok.empty() || firstTok.front() != '"') {
                    cout << "Invalid operation\n";
                    continue;
                }
                string content;
                bool ok = true;
                if (firstTok.size() >= 2 && firstTok.back() == '"') {
                    content = firstTok.substr(1, firstTok.size() - 2);
                } else {
                    content = firstTok.substr(1);
                    string more;
                    bool closed = false;
                    while (cin >> more) {
                        if (!more.empty() && more.back() == '"') {
                            content += ' ' + more.substr(0, more.size() - 1);
                            closed = true;
                            break;
                        } else {
                            content += ' ' + more;
                        }
                    }
                    if (!closed) ok = false;
                }
                if (!ok) cout << "Invalid operation\n";
                else it->second.back().sv += content;
            }
            if (!valid) cout << "Invalid operation\n";
            continue;
        } else if (op == "Print") {
            string name;
            if (!(cin >> name)) { valid = false; }
            if (valid) {
                auto it = table.find(name);
                if (it == table.end() || it->second.empty()) valid = false;
                else {
                    if (it->second.back().is_int) cout << name << ':' << (long long)it->second.back().iv << '\n';
                    else cout << name << ':' << it->second.back().sv << '\n';
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
