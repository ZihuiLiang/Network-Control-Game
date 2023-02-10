#include <algorithm>
#include <cstdio>
using namespace std;
long long f[201][201];
long long v[201];
int main() {
    f[0][0] = 1;
    for (int i = 0; i < 200;i ++)
        for (int j = 0; j <= i; j ++)
            for (int k = j + 1; k +i <= 200; k ++)
                f[k+i][k] += f[i][j];
    v[0] = 1;
    for (int i = 1; i <= 200; i ++) {
        v[i] = v[i - 1];
        for (int j = 0; j <= i; j ++)
            v[i] += f[i][j];
        printf("%d %lld\n", i, v[i]);
    }
    return 0;
}