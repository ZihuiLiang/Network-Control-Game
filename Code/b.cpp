#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <map>
#include <cmath>
using namespace std;
map<vector<int>, int> f;
void clear(vector<int> &d) {
    sort(d.begin(), d.end());
    int l = 0;
    for (int i = 1; i < d.size(); i ++)
        if (l == -1 || d[i] != d[l] )
            d[++l] = d[i];
        else l --;
    d.resize(l + 1);
}
void input() {
    freopen("out0", "r", stdin);
    int n;
    while (scanf("%d", &n) != EOF) {
        vector<int> d={};
        while (n --) {
            int x;
            scanf("%d", &x);
            d.push_back(x);
        }
        f[d] = 0;
    }
    fclose(stdin);
    freopen("out1", "r", stdin);
    while (scanf("%d", &n) != EOF) {
        vector<int> d={};
        while (n --) {
            int x;
            scanf("%d", &x);
            d.push_back(x);
        }
        f[d] = 1;
    }
    fclose(stdin);
    freopen("out2", "r", stdin);
    while (scanf("%d", &n) != EOF) {
        vector<int> d={};
        while (n --) {
            int x;
            scanf("%d", &x);
            d.push_back(x);
        }
        f[d] = 2;
    }
    fclose(stdin);
    freopen("out3", "r", stdin);
    while (scanf("%d", &n) != EOF) {
        vector<int> d={};
        while (n --) {
            int x;
            scanf("%d", &x);
            d.push_back(x);
        }
        f[d] = 3;
    }
    fclose(stdin);
}
int main() {
    input();
    for (int i = 1; i + i + 6 <= 100; i ++) {
        if (f.find({i, i + 6}) != f.end())
            printf("%d %d\n", i, f[{i, i + 6}]);
    }
    return 0;
}