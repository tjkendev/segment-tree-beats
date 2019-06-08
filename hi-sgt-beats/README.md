# Historic Information をSegment Tree Beatsで処理する実装

## 実装一覧

### `sgt-beats-hist-0.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の b_i の総和を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)

計算量: O(N log N + M log^2 N)

### `sgt-beats-hist-0-1.cpp`

長さNの数列Aと数列Aに等しい数列B, 数列Cに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の b_i の総和を求める
- l ≤ i < r の c_i の総和を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)
- (各クエリごとに全てのiについて c_i を min(c_i, a_i) に更新)

計算量: O(N log N + M log^2 N)

### `sgt-beats-hist-0-2.cpp`

長さNの数列Aと数列Aに等しい数列B, 数列C, 数列Sに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の b_i の総和を求める
- l ≤ i < r の c_i の総和を求める
- l ≤ i < r の s_i の総和を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)
- (各クエリごとに全てのiについて c_i を min(c_i, a_i) に更新)
- (各クエリごとに全てのiについて s_i を s_i + a_i に更新)

計算量: O(N log N + M log^2 N)

### `sgt-beats-hist-0-3.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の b_i の総和を求める
- l ≤ i < r の b_i の最大値を求める
- l ≤ i < r の b_i の最小値を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)

計算量: O(N log N + M log^2 N)

### `sgt-beats-hist-1.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の b_i の総和を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)

計算量: O(N log^2 N + M log^3 N)

### `sgt-beats-hist-1-1.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の b_i の総和を求める
- l ≤ i < r の b_i の最大値を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)

計算量: O(N log^2 N + M log^3 N)

### `sgt-beats-hist-1-3.cpp`

長さNの数列Aと数列Aに等しい数列Cに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の c_i の総和を求める
- l ≤ i < r の c_i の最小値を求める
- (各クエリごとに全てのiについて c_i を min(c_i, a_i) に更新)

計算量: O(N log^2 N + M log^3 N)

### `sgt-beats-hist-2.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r について a_i の値を x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の b_i の総和を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)

計算量: O(N log^2 N + M log^3 N)

### `sgt-beats-hist-3.cpp`

長さNの数列Aと数列Aに等しい数列B, 数列Cに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r について a_i の値を x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の b_i の総和を求める
- l ≤ i < r の c_i の総和を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)
- (各クエリごとに全てのiについて c_i を min(b_i, a_i) に更新)

計算量: O(N log^2 N + M log^3 N)

### `sgt-beats-hist-3-1.cpp`

長さNの数列Aと数列Aに等しい数列B, 数列Cに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r について a_i の値を x に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の b_i の総和を求める
- l ≤ i < r の b_i の最大値を求める
- l ≤ i < r の c_i の総和を求める
- l ≤ i < r の c_i の最小値を求める
- (各クエリごとに全てのiについて b_i を max(b_i, a_i) に更新)
- (各クエリごとに全てのiについて c_i を min(b_i, a_i) に更新)

計算量: O(N log^2 N + M log^3 N)
