#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <cmath>
using namespace std;
int SEARCH_SIZE = 100;
set<string > f[4];
void clear(vector<int> &d) {
    if (d.empty()) return;
    sort(d.begin(), d.end());
    int l = 0;
    for (int i = 1; i < d.size(); i ++)
        if (l == -1 || d[i] != d[l] )
            d[++l] = d[i];
        else l --;
    d.resize(l + 1);
}
string to_string(vector<int> d) {
    string s;
    s.resize(d.size());
    for (int i = 0; i < d.size();  i ++)
        s[i] = d[i];
    return s;
}
int work(vector<int> d) {
    if (d.size() == 0) return 0;
    string w = to_string(d);
    for (int i = 0; i < 4; i ++)
        if (f[i].find(w) != f[i].end()) return i;
    vector<int> dd;
    int v = 0;
    int sum = 0;
    for (auto u:d) sum += u;
    if (sum & 1) {
        v = 1;
        for (int i = 0; i < d.size() && v != 3; i ++) {
            for (int j = 2; j <= (d[i] + 1) / 2; j ++) {
                vector<int> u = dd;
                if (j > 2) u.push_back(j - 2);
                if (j < d[i] - 1) u.push_back(d[i] - 1 - j);
                for (int k = i + 1; k < d.size(); k ++) u.push_back(d[k]);
                sort(u.begin(), u.end());
                clear(u);
                string w = to_string(u);
                if (u.empty() || f[0].find(w) != f[0].end()) {
                    v = 3;
                    break;
                }
            }
            dd.push_back(d[i]);
        }
    } else {
        for (int i = 0; i < d.size() && v != 2; i ++) {
            if (d[i] != 1) {
                for (int j = 1; j <= (d[i] + 1) / 2; j ++) {
                    vector<int> u = dd;
                    if (j > 2) u.push_back(j - 2);
                    if (j < d[i] - 1) u.push_back(d[i] - 1 - j);
                    for (int k = i + 1; k < d.size(); k ++) u.push_back(d[k]);
                    sort(u.begin(), u.end());
                    clear(u);
                    string w = to_string(u);
                    if (j == 1) {
                        if (u.empty() || f[0].find(w) != f[0].end()) {
                            v = 2;
                            break;
                        } 
                    } else {
                        if (f[1].find(w) != f[1].end()) {
                            v = 2;
                            break;
                        }
                    }
                }
            }
            dd.push_back(d[i]);
        }
    }
    f[v].insert(w);
    return v;
}
int cnt;
int cnt_1;
void split(int n, int mx, vector<int> d) { // generate linear forests with n vertices
    if ((mx + 1) * mx / 2 < n) return;
    if (!n) {
        clear(d);
        int v = work(d);
        return;
    }
    for (int i = 1; i <= min(n, mx); i ++) {
        d.push_back(i);
        split(n - i, i - 1, d);
        d.pop_back();
    }
}
int main() {
    for (int i = 1;  i <= SEARCH_SIZE; i ++) {
        split(i, i, {}), printf("Finish:%d\n", i);
        if (f[0].size() > 3) {
            printf("more than 3!!!");
            break;
        }
    }
    freopen("game0", "w",stdout); // F(G)=0
    for (auto u:f[0]) {
        printf("%lu ", u.size());
        for (int i = 0; i < u.size(); i ++)
            if (u[i] <= 0)
                printf("%d ", (int)u[i] + 256);
            else 
                printf("%d ", u[i]);
        puts("");
    }
    fclose(stdout);
    freopen("game1", "w",stdout); // F(G)=1
    for (auto u:f[1]) {
        printf("%lu ", u.size());
        for (int i = 0; i < u.size(); i ++)
            if (u[i] <= 0)
                printf("%d ", (int)u[i] + 256);
            else 
                printf("%d ", u[i]);
        puts("");
    }
    fclose(stdout);
    freopen("game2", "w",stdout); // F(G)=2
    for (auto u:f[2]) {
        printf("%lu ", u.size());
        for (int i = 0; i < u.size(); i ++)
            if (u[i] <= 0)
                printf("%d ", (int)u[i] + 256);
            else 
                printf("%d ", u[i]);
        puts("");
    }
    fclose(stdout);
    freopen("game3", "w",stdout); // F(G)=3
    for (auto u:f[3]) {
        printf("%lu ", u.size());
        for (int i = 0; i < u.size(); i ++)
            if (u[i] <= 0)
                printf("%d ", (int)u[i] + 256);
            else 
                printf("%d ", u[i]);
        puts("");
    }
    fclose(stdout);
    return 0;
}