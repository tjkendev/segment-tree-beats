# SGT Beatsの応用的な実装

## 実装一覧

### `sgt-beats-4(-X).cpp`

長さNの数列AとBに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について b_i の値を min(b_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r について b_i の値を b_i + x に更新
- l ≤ i < r の中の a_i + b_i の最大値を求める

計算量: O(N log N + M log^2 N)

### `sgt-beats-5.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の中の a_i の最小値を求める
- l ≤ i < r の a_i の総和を求める
- l ≤ i < r の b_i の総和を求める
- (各クエリごとに a_i が変化した i について b_i に1加算する)

計算量: O((N+M) log N)

### `sgt-beats-5-2.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を max(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l ≤ i < r の b_i の総和を求める
- (各クエリごとに a_i が変化した i について b_i に1加算する)

計算量: O(N log N + M log^2 N)

### `sgt-beats-6.cpp`

長さNの数列Aと数列Aに等しい数列Bに対し、以下のクエリを合計M回処理する

- l ≤ i < r について a_i の値を min(a_i, x) に更新
- l ≤ i < r について a_i の値を a_i + x に更新
- l < r について gcd(a_l, a_{l+1}, ..., a_{r-1}) を求める
- l ≤ i < r の中の a_i の最大値を求める
- l ≤ i < r の a_i の総和を求める

計算量: O(N log N + M log^2 N)
