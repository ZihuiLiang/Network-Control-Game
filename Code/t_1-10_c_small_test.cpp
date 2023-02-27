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
#include <pthread.h>
#include <semaphore.h>
#include "my_timer.h"
using namespace std;
int T=3;
int LEN=T*2+1;
int MAX_SIZE = 50;
int SEARCHING_SIZE = 100;
vector<string> zero_graph; // store the graphs G with F(G)=0
const int BITS = 25; // BITS is larger, the memory is larger and the code runs more efficient. 
const int mod = 1 << BITS;
const int thread_num = 128;
const int thread_mask = thread_num - 1;
vector<pair<string, int> > f[mod]; // f stores (G,v|n) with F(G)=v and |G|=n. Note that the integer value is a compressed value with v and n.
vector<pair<vector<int>, pair<int, int> > > configuration_queue[thread_num]; // the generated configuration queue for each thread
struct task_signal{ // control the distribution of the computation
    sem_t distribute_compute, gather_compute;
    sem_t distribute_store, gather_store;
    pthread_mutex_t zero_graph_mutex;
    void init() {
        if (sem_init(&distribute_compute, 0, 0)) {
            printf("sem initialization failed!\n");
            exit(1);
        }
        if (sem_init(&distribute_store, 0, 0)) {
            printf("sem initialization failed!\n");
            exit(1);
        }
        if (sem_init(&gather_compute, 0, 0)) {
            printf("sem initialization failed!\n");
            exit(1);
        }
        if (sem_init(&gather_store, 0, 0)) {
            printf("sem initialization failed!\n");
            exit(1);
        }
        if (pthread_mutex_init(&zero_graph_mutex, NULL)) {
            printf("mutex initialization failed!\n");
            exit(1);
        }
    }

    void distribute_compute_master() {
        sem_post(&distribute_compute);
    }
    void distribute_compute_slave() {
        sem_wait(&distribute_compute);
    }
    void gather_compute_master() {
        sem_wait(&gather_compute);
    }
    void gather_compute_slave() {
        sem_post(&gather_compute);
    }

    void distribute_store_master() {
        sem_post(&distribute_store);
    }
    void distribute_store_slave() {
        sem_wait(&distribute_store);
    }
    void gather_store_master() {
        sem_wait(&gather_store);
    }
    void gather_store_slave() {
        sem_post(&gather_store);
    }

    void add_zero_graph(string v) {
        pthread_mutex_lock(&zero_graph_mutex);
        zero_graph.push_back(v);
        pthread_mutex_unlock(&zero_graph_mutex);
    }
}TS;
void set_environment(int _T, int _MAX_SIZE) {
    T=_T;
    LEN=T*2+1;
    zero_graph.clear();
    MAX_SIZE=_MAX_SIZE;
    int max_last_size = 0;
    for (int i = 0; i < mod; i ++) max_last_size = max(max_last_size, (int)f[i].size()), f[i].clear();
}  
void clear(vector<int> &d) { // apply DRT(G,D_sym)
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
bool cmp(const pair<string, int>&a, const pair<string, int>&b) {
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
void *single_thread_search(void * arg) { // a thread for searching partial cases
    int id = *((int*)arg);
    while (1) {
        TS.distribute_compute_slave();
        for (auto &x:configuration_queue[id]) { // compute F(G) for each configuration
            vector<int> d = x.first;
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
            x.second = {v, sum};
        }
        TS.gather_compute_slave();
        TS.distribute_store_slave();
        for (auto &x:configuration_queue[id]) {  // store F(G) in f
            insert(to_string(x.first), x.second.first,x.second.second);
            if (x.second.first == 0) TS.add_zero_graph(to_string(x.first));
        }
        TS.gather_store_slave();
    }
}
int cnt;
int cnt_1;
void split(int n, int mn, vector<int> d) { // generate linear forests with n vertices
    if (!n) {
        int v = 0;
        for (auto u:d)
            v = v * 171 + u;
        configuration_queue[v & thread_mask].push_back({d,{0, 0}});
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
void single_search(int _T, int _MAX_SIZE) { // generate linear forests and distribute them to each thread for computing F(G)
    set_environment(_T, _MAX_SIZE);
    for (int i = 1;  i <= MAX_SIZE; i ++) {
        split(i, 1, {});
        for (int j = 0; j < thread_num; j ++) TS.distribute_compute_master();
        for (int j = 0; j < thread_num; j ++) TS.gather_compute_master();
        for (int j = 0; j < thread_num; j ++) TS.distribute_store_master();
        for (int j = 0; j < thread_num; j ++) TS.gather_store_master();
        for (int j = 0; j < thread_num; j ++) configuration_queue[j].clear();
    }
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
long long check_sum(int A, int B) {
    set<string> _f[LEN+1];
    long long v = 0;
    for (int i = 0; i < mod; i ++)
        for (auto u:f[i]) {
            _f[u.second>>10].insert(u.first);
        }
    for (int i = 0; i <= LEN; i ++) 
        for (auto u:_f[i]) {
            for (auto w:u)
                v = v * A + w;
            v = v * B + i;
        }

    return v;
}
int main() {
    setbuf(stdout, NULL);
    TS.init();
    pthread_t th[thread_num];
    int arg[thread_num];
    for (int i = 0; i < thread_num; i ++) {
        arg[i] = i;
        int ret = pthread_create(&th[i], NULL, single_thread_search, &arg[i]);
        if (ret != 0) {
            printf("Create thread error!\n");
            return -1;
        }
    }
    printf("This is the main process.\n");
    printf("searching size:%d\n", SEARCHING_SIZE);
    MyTimer timer(MyTimer:: SECOND);
    for (int i = 1; i <= 10; i ++) {
        printf("check t=%d:", i);
        fflush(stdout);
        timer.SetRecordTimePoint();
        single_search(i, SEARCHING_SIZE);
        if (check_draw_characteristics(1)) printf("YES");
        else printf("NO");
        puts("");
        printf("%.3fs\n", timer.GetRecordDuration());
        fflush(stdout);
    }
    exit(0);
    for (int i = 0; i < thread_num; i ++) {
        int ret = pthread_join(th[i], (void**)NULL);
        if (ret != 0) {
            printf("Join thread error!\n");
            return -1;
        }
    }
    return 0;
}