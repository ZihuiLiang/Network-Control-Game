The code is to search for the draw configurations and output these configurations for network-control games where parameter $t=T$  and the size of configurations is bounded by SEARCH_SIZE (see src/main.rs). The current version sets $T=1$ and SEARCH_SIZE=80. One can run the code within 10 seconds to get the result. One can update $T$ and SEARCH_SIZE for any preferred test.

Also, we provide an API (NetworkControl::compute(paths, t)) for computing $F(G)$ in network-control game on paths with parameter $t=1$ and determining the winner of network-control game on paths with parameter $t>1$. We use the searching results to verify the API.

If $t=1$, NetworkControl::compute(paths, t)=F(paths), otherwise NetworkControl::compute(paths,t)>0 iff player 0 wins the game.

The way to install rust: https://www.rust-lang.org/learn/get-started

The way to compile and run the code:

```bash
cargo run --release
```

