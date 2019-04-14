# "Segment Tree Beats" の実装

- 各プログラムで、ランダム生成されたクエリによるテストを実行します
- 学習のための実装なので、無駄実装がちょこちょこあります

## ソースコードの説明

### 区間最大値(最小値)クエリ

#### `sgt-beats-0.cpp`

- range chmin query
- range maximum query
- range sum query

計算量: O(M log N)

#### `sgt-beats-1.cpp`

- range chmin query
- range chmax query
- range minimum query
- range maximum query
- range sum query

計算量: O(M log^2 N)

#### `sgt-beats-2.cpp`

- range chmin query
- range chmax query
- range minimum query
- range maximum query
- range sum query
- range add query

計算量: O(M log^2 N)

#### `sgt-beats-3.cpp`

- range chmin query
- range chmax query
- range minimum query
- range maximum query
- range sum query
- range add query
- range update query

計算量: O(M log^2 N)

### 区間過去最大値(最小値)クエリ

#### `sgt-hist-0.cpp`

これは普通の遅延セグ木

- range update query
- range add query
- range maximum query
- range sum query
- range histric maximum query

計算量: O(M log N)

#### `sgt-hist-1.cpp`

これも普通の遅延セグ木

- range update query
- range add query
- range maximum query
- range minimum query
- range sum query
- range histric maximum query
- range histric minimum query

計算量: O(M log N)

## 参考

- [A simple introduction to "Segment tree beats"](https://codeforces.com/blog/entry/57319)
- [Segment Tree Beats - An introduction](http://codingwithrajarshi.blogspot.com/2018/03/segment-tree-beats-introduction.html)
- [区间最值操作与历史最值问题](http://www.doc88.com/p-6744902151779.html)