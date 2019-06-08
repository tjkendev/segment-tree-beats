# SGT Beatsの基本的な実装

## 実装一覧

### `sgt-0.cpp`

長さNの数列Aに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r の中の a_i の最大値を求める

従来の遅延セグ木の実装

計算量: O(M log N)

### `sgt-beats-0(-X).cpp`

長さNの数列Aに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の a_i の総和を求める

計算量: O((N+M) log N)

### `sgt-beats-1.cpp`

長さNの数列Aに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の a_i の総和を求める

計算量: O((N+M) log N)

### `sgt-beats-2.cpp`

長さNの数列Aに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の a_i の総和を求める
- l ≤ i < r について a_i の値を a_i + x に更新

計算量: O(N log N + M log^2 N)

### `sgt-beats-3(-X).cpp`

長さNの数列Aに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の a_i の総和を求める
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r について a_i の値を x に更新

計算量: O(N log N + M log^2 N)
