#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <map>
#include <cmath>
using namespace std;
map<vector<int>, int> f;
int SEARCH_SIZE = 50;
void clear(vector<int> &d) {
    sort(d.begin(), d.end());
    int l = 0;
    for (int i = 1; i < d.size(); i ++)
        if (l == -1 || d[i] != d[l] )
            d[++l] = d[i];
        else l --;
    d.resize(l + 1);
}
int work(vector<int> d) {
    if (d.size() == 0) return 0;
    auto q = f.find(d);
    if (q != f.end()) return q->second;
    vector<int> dd;
    int v = 0;
    for (int i = 0; i < d.size(); i ++) {
        for (int j = 1; j <= d[i]; j ++) {
            vector<int> u = dd;
            int w = 1;
            if (j != 1) w ++;
            if (j != d[i]) w ++;
            if (j > 2) u.push_back(j - 2);
            if (j < d[i] - 1) u.push_back(d[i] - 1 - j);
            for (int k = i + 1; k < d.size(); k ++) u.push_back(d[k]);
            sort(u.begin(), u.end());
            clear(u);
            v = max(v, w - work(u));
        }
        dd.push_back(d[i]);
    }
    f[d] = v;
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
    printf("searching size:%d\n", SEARCH_SIZE);
    work({1, 3, 4});
    for (int i = 1; i <= SEARCH_SIZE; i ++)
        split(i, i, {}), printf("Finish:%d\n", i);
    freopen("game0", "w",stdout); // F(G)=0
    for (auto u:f)
        if (u.second == 0) {
            printf("%lu ", u.first.size());
            for (auto v:u.first)
                printf("%d ", v);
            puts("");
        }
    fclose(stdout);
    freopen("game1", "w",stdout); // F(G)=1
    for (auto u:f)
        if (u.second == 1) {
            printf("%lu ", u.first.size());
            for (auto v:u.first)
                printf("%d ", v);
            puts("");
        }
    fclose(stdout);
    freopen("game2", "w",stdout); // F(G)=2
    for (auto u:f)
        if (u.second == 2) {
            printf("%lu ", u.first.size());
            for (auto v:u.first)
                printf("%d ", v);
            puts("");
        }
    fclose(stdout);
    freopen("game3", "w",stdout); // F(G)=3
    for (auto u:f)
        if (u.second == 3) {
            printf("%lu ", u.first.size());
            for (auto v:u.first)
                printf("%d ", v);
            puts("");
        }
    fclose(stdout);
    return 0;
}