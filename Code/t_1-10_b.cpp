#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <string>
#include <cstring>
#include <cmath>
using namespace std;
int T=3;
int LEN=T*2+1;
int MAX_SIZE = 50;
int SEARCHING_SIZE = 70;
vector<string> zero_graph;
const int mod = 1 << 15;
vector<pair<string, int> > f[mod];
void set_environment(int _T, int _MAX_SIZE) {
    T=_T;
    LEN=T*2+1;
    zero_graph.clear();
    MAX_SIZE=_MAX_SIZE;
    int max_last_size = 0;
    for (int i = 0; i < mod; i ++) max_last_size = max(max_last_size, (int)f[i].size()), f[i].clear();
}  
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
vector<int> to_vec(string s) {
    vector<int> d(s.size());
    for (int i = 0; i < s.size(); i ++)
        d[i] = s[i];
    return d;
}
bool cmp(const pair<string, int> a, const pair<string, int> b) {
    if ((a.second & ((1<< 10)-1)) != (b.second & ((1<< 10)-1))) return (a.second & ((1<< 10)-1)) < (b.second & ((1<< 10)-1));
    return a.first < b.first;
}
int get(string s) {
    int v = 0, sum = 0;
    for (auto u:s)
        v = v * 171 + u, sum += u;
    v &= (mod - 1);
    int x = lower_bound(f[v].begin(), f[v].end(), make_pair(s, sum), cmp) - f[v].begin();   
    return (f[v][x].second >> 10);
}
void insert(string s, int w, int sum) {
    int v = 0;
    for (auto u:s)
        v = v * 171 + u;
    v &= (mod - 1);
    f[v].push_back({s, (w << 10) + sum});
}
int work(vector<int> d) { // get F(G) G={d[0],...,d[n-1]}
    if (d.size() == 0) return 0;
    vector<int> dd;
    int v = 0;
    int sum = 0;
    for (auto u:d) sum += u;
    v = (sum & 1);
    for (int i = 0; i < d.size() && v != LEN; i ++) {
        for (int j = 1; j <= (d[i] + 1) / 2 &&  v != LEN; j ++) {
            vector<int> u = dd;
            int vv = 1;
            if (j > T+1) u.push_back(j - T - 1), vv += T;
            else vv += j - 1;
            if (j < d[i] - T) u.push_back(d[i] - j - T), vv += T;
            else vv += d[i] - j;
            if (vv <= v) continue;
            for (int k = i + 1; k < d.size(); k ++) u.push_back(d[k]);
            sort(u.begin(), u.end());
            clear(u);
            if (u.empty()) {
                v = vv;
                continue;
            }
            v = max(v, vv - get(to_string(u)));
        }
        dd.push_back(d[i]);
    }
    insert(to_string(d),v, sum);
    if (!v) zero_graph.push_back(to_string(d));
    return v;
}
int cnt;
int cnt_1;
void split(int n, int mn, vector<int> d) { // generate linear forests with n vertices
    if (!n) { 
        int v = work(d);
        return;
    }
    if (mn > n) return;
    for (int i = mn; i<= n / 2 ; i ++) {
        d.push_back(i);
        split(n - i, i + 1, d);
        d.pop_back();
    }
    d.push_back(n);
    split(0, n+1,d);
}
void search(bool notice) {
    for (int i = 1;  i <= MAX_SIZE; i ++) {
        split(i, 1, {});
        if (notice) printf("Finish:%d\n", i);
    }
}
void single_search(int _T, int _MAX_SIZE, bool notice) {
    set_environment(_T, _MAX_SIZE);
    search(notice);
}
bool same_vec(vector<int> a, vector<int> b) {
    if (a.size()!=b.size()) return 0;
    for (int i = 0; i < a.size(); i ++)
        if (a[i] != b[i]) return 0;
    return 1;
}
bool check_draw_characteristics(bool notice) { // check draw configurations
    bool check=0;
    sort(zero_graph.begin(), zero_graph.end());
    if (T==1) {
        if (zero_graph.size()==3) {
            check=1;
            vector<int> u=to_vec(zero_graph[0]);
            check&=same_vec(u, {1,3,4});
            if (notice) {
                for (auto v:u)
                    printf("%d ", v);
                printf("|");
            }
            u=to_vec(zero_graph[1]);
            check&=same_vec(u, {1,4,5,8});
            if (notice) {
                for (auto v:u)
                    printf("%d ", v);
                printf("|");
            }
            u=to_vec(zero_graph[2]);
            check&=same_vec(u, {3,5,8});
            if (notice) {
                for (auto v:u)
                    printf("%d ", v);
                printf("|");
            }
        }
    } else {
        if (zero_graph.size()==1) {
            check=1;
            auto u = to_vec(zero_graph[0]);
            check&=same_vec(to_vec(zero_graph[0]), {1, T*2+1, T*2+2}); 
            if (notice)
                for (auto v:u)
                    printf("%d ", v);
        }
    }
    return check;
}
void check_f() {
    printf("searching size:%d\n", SEARCHING_SIZE);
    for (int i = 1; i <= 10; i ++) {
        printf("check t=%d:", i);
        fflush(stdout);
        double s = clock();
        single_search(i, SEARCHING_SIZE, 0);
        if (check_draw_characteristics(1)) printf("YES");
        else printf("NO");
        puts("");
        printf("%.3fs\n", (clock() - s)/ CLOCKS_PER_SEC);
        fflush(stdout);
    }
}
int main() {
    check_f();
    return 0;
}