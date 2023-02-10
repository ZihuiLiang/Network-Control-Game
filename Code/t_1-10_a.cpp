#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <set>
#include <cmath>
using namespace std;
int T=3;
int LEN=T*2+1;
int MAX_SIZE = 50;
int SEARCHING_SIZE = 100;
set<string> f[100];
void set_environment(int _T, int _MAX_SIZE) {
    T=_T;
    LEN=T*2+1;
    MAX_SIZE=_MAX_SIZE;
    for (int i = 0; i < 100; i ++) f[i].clear();
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
int work(vector<int> d) {
    if (d.size() == 0) return 0;
    vector<int> dd;
    int v = 0;
    int sum = 0;
    for (auto u:d) sum += u;
    v = (sum & 1);
    string w = to_string(d);
    for (int i = v; i <= LEN; i += 2)
        if (f[i].find(w) != f[i].end()) return i;
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
            v = max(v, vv - work(u));
        }
        dd.push_back(d[i]);
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
void search(bool notice) {
    for (int i = 1;  i <= MAX_SIZE; i ++) {
        split(i, i, {});
        if (notice) printf("Finish:%d\n", i);
    }
}
void output() {
    char s[10];
    for (int result=0; result<=LEN; result ++) {
        sprintf(s, "tout%d", result);
        freopen(s, "w",stdout);
        for (auto u:f[result]) {
            printf("%lu ", u.size());
            for (int i = 0; i < u.size(); i ++)
                if (u[i] <= 0)
                    printf("%d ", (int)u[i] + 256);
                else 
                    printf("%d ", u[i]);
            puts("");
        }
        fclose(stdout);
    }
    
}

void output0() {
    freopen("tout0", "w",stdout);
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
} 
void single_search(int _T, int _MAX_SIZE, bool notice) {
    set_environment(_T, _MAX_SIZE);
    search(notice);
}
void single_check(int _T, int _MAX_SIZE, bool notice) {
    single_search(_T, _MAX_SIZE, notice);
    output0();
   // output();
}
bool same_vec(vector<int> a, vector<int> b) {
    if (a.size()!=b.size()) return 0;
    for (int i = 0; i < a.size(); i ++)
        if (a[i] != b[i]) return 0;
    return 1;
}
bool check_draw_characteristics(bool notice) {
    bool check=0;
    if (T==1) {
        if (f[0].size()==3) {
            check=1;
            auto tmp=f[0].begin();
            vector<int> u=to_vec(*tmp);
            check&=same_vec(u, {1,3,4});
            if (notice) {
                for (auto v:u)
                    printf("%d ", v);
                printf("|");
            }
            tmp++;
            u=to_vec(*tmp);
            check&=same_vec(u, {1,4,5,8});
            if (notice) {
                for (auto v:u)
                    printf("%d ", v);
                printf("|");
            }
            tmp++;
            u=to_vec(*tmp);
            check&=same_vec(u, {3,5,8});
            if (notice) {
                for (auto v:u)
                    printf("%d ", v);
                printf("|");
            }
        }
    } else {
        if (f[0].size()==1) {
            check=1;
            auto u=to_vec(*f[0].begin());
            check&=same_vec(to_vec(*f[0].begin()), {1, T*2+1, T*2+2}); 
            if (notice)
                for (auto v:u)
                    printf("%d ", v);
        }
    }
    return check;
}
long long check_sum(int A, int B) {
    long long v = 0;
    for (int i = 0; i <= LEN; i ++) 
        for (auto u:f[i]) {
            for (auto w:u)
                v = v * A + w;
            v = v * B + i;
        }

    return v;
}
void check_f() {
    for (int i = 1; i <= 10; i ++) {
        printf("check t=%d:", i);
        double s = clock();
        single_search(i, SEARCHING_SIZE, 0);
        if (check_draw_characteristics(1)) printf("YES");
        else printf("NO");
        puts("");
        //printf("%lld\n", check_sum(1231, 1239));
        printf("%.3fs\n", (clock() - s)/ CLOCKS_PER_SEC);
    }
}
int main() {
    //single_check(1);
    check_f();
    return 0;
}