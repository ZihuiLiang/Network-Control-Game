# Introduction

This is a code library for searching draw configuration patterns of network control games in linear forests. The key algorithm is memorized search.

Note that the algorithm only searchs configurations $G$ with $G=DRT(G,D_{sym})$.

Test system: 92 threads, 192 GB memory, Ubuntu 18.04.

t_1_a.cpp, t_1_b.cpp and t_1_c.cpp work on searching the basic result (n<=100) for network control game with $t=1$ and output the results.  t_1_c.cpp is the most effcient one.

For larger graphs, we verify the pattern of draw configurations and omit the output of the classifications of each configurations.

t_1-10_a.cpp, t_1-10_b.cpp, t_1-10_c.cpp work on verification of draw configurations for network control game with $t\in [1,10]$. By applying efficient datastructures (e.g., hash table) and multithread technique, t_1-10_c.cpp is the most efficient one.

Note that if one wants to verify the result of large graphs efficiently, the code needs large memory and many threads. For example, to check the bound of $180$,  we run t_1-10.c.cpp with 192GB memory and 92 threads for about 2 hours. Also we provide a test version, t_1-10_c_small_test.cpp which verifies graphs $G$ with $|G|\le 100$. Run it with 8GB memory and 16-thread system. One can get the result in 1 or 2 minutes.

The way to run these codes is shown as follows:

```bash
g++ t_1_a.cpp -o t_1_a -O2 -std=c++11
./t_1_a
//output 4 files. file "gamei" contains the configurations G with n(G)<=50 such that F(G)=i

g++ t_1_b.cpp -o t_1_b -O2 -std=c++11
./t_1_b
//output 4 files. file "gamei" contains the configurations G with n(G)<=50 such that F(G)=i

g++ t_1_c.cpp -o t_1_c -O2 -std=c++11
./t_1_c
//output 4 files. file "gamei" contains the configurations G with n(G)<=100 such that F(G)=i

g++ t_1-10_a.cpp -o t_1-10_a -O2 -std=c++11
./t_1-10_a
//verify the games with t in [1,10] on graphs G with n(G)<=100

g++ t_1-10_b.cpp -o t_1-10_b -O2 -std=c++11
./t_1-10_b
//verify the games with t in [1,10] on graphs G with n(G)<=100


g++ t_1-10_c.cpp my_timer.cpp -o t_1-10_c -O2 -std=c++11 -pthread
./t_1-10_c 
//verify the games with t in [1,10] on graphs G with n(G)<=180

g++ t_1-10_c_small_test.cpp my_timer.cpp -o t_1-10_c_small_test -O2 -std=c++11 -pthread
./t_1-10_c_small_test
//verify the games with t in [1,10] on graphs G with n(G)<=100
```