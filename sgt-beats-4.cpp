#include<algorithm>
#include<random>
#include<cassert>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について、 A_i の値を min(A_i, x) に更新
// - l<=i<r について、 B_i の値を min(B_i, x) に更新
// - l<=i<r について、 A_i の値を A_i + x に更新
// - l<=i<r について、 B_i の値を B_i + x に更新
// - l<=i<r の A_i + B_i の最大値を求める

#define N 10003

class SegmentTree {
  const ll inf = 1e18;
  struct Pair {
    ll a, b;
    ll sum() const { return a+b; }

    bool operator<(const Pair& other) const {
      return this->sum() < other.sum();
    }
  };

  int n, n0;
  // sum_ma[k]: ノードkの区間において、A_iが最大値で(A_i + B_i)が最大となるペア(A_i, B_i)を保持
  // sum_a[k]: ノードkの区間において、A_iが最大値未満で(A_i + B_i)が最大となるペア(A_i, B_i)を保持
  Pair sum_ma[4*N], sum_a[4*N];
  // sum_mb[k]: ノードkの区間において、B_iが最大値で(A_i + B_i)が最大となるペア(A_i, B_i)を保持
  // sum_b[k]: ノードkの区間において、B_iが最大値未満で(A_i + B_i)が最大となるペア(A_i, B_i)を保持
  Pair sum_mb[4*N], sum_b[4*N];
  // sum_n[k]: ノードkの区間において、A_iが最大値未満かつB_iが最大値未満を満たす中で、(A_i + B_i)が最大となるペア(A_i, B_i)を保持
  Pair sum_n[4*N];

  // AとBに関する最大値、二番目の最大値
  ll max_va[4*N], smax_va[4*N];
  ll max_vb[4*N], smax_vb[4*N];
  // 遅延させている加算値
  ll ladd_a[4*N], ladd_b[4*N];

  // ノードの各情報を更新する
  void update_node_max(int k, ll xa, ll xb) {
    // sum_ma[k].a, sum_mb[k].a, sum_b[k].a をchmin更新
    // sum_a[k].a, sum_n[k].a は最大値にならないため更新しなくてよい
    if(xa < max_va[k]) max_va[k] = sum_ma[k].a = xa;

    if(xa < sum_mb[k].a) sum_mb[k].a = xa;
    if(xa < sum_b[k].a) sum_b[k].a = xa;


    // sum_mb[k].b, sum_ma[k].b, sum_a[k].b をchmin更新
    // sum_b[k].b, sum_n[k].b は最大値にならないため更新しなくてよい
    if(xb < max_vb[k]) max_vb[k] = sum_mb[k].b = xb;

    if(xb < sum_ma[k].b) sum_ma[k].b = xb;
    if(xb < sum_a[k].b) sum_a[k].b = xb;
  }

  // 区間内のA_iにxaを加算し、区間内のB_iにxbを加算する
  void addall(int k, ll xa, ll xb) {
    if(xa != 0) {
      sum_ma[k].a += xa;
      if(sum_a[k].a != -inf) sum_a[k].a += xa;

      sum_mb[k].a += xa;
      if(sum_b[k].a != -inf) sum_b[k].a += xa;

      if(sum_n[k].a != -inf) sum_n[k].a += xa;

      max_va[k] += xa;
      if(smax_va[k] != -inf) smax_va[k] += xa;
      ladd_a[k] += xa;
    }

    if(xb != 0) {
      sum_ma[k].b += xb;
      if(sum_a[k].b != -inf) sum_a[k].b += xb;

      sum_mb[k].b += xb;
      if(sum_b[k].b != -inf) sum_b[k].b += xb;

      if(sum_n[k].b != -inf) sum_n[k].b += xb;

      max_vb[k] += xb;
      if(smax_vb[k] != -inf) smax_vb[k] += xb;
      ladd_b[k] += xb;
    }
  }

  // pushdown
  void push(int k) {
    if(ladd_a[k] != 0 || ladd_b[k] != 0) {
      addall(2*k+1, ladd_a[k], ladd_b[k]);
      addall(2*k+2, ladd_a[k], ladd_b[k]);
      ladd_a[k] = ladd_b[k] = 0;
    }

    update_node_max(2*k+1, max_va[k], max_vb[k]);
    update_node_max(2*k+2, max_va[k], max_vb[k]);
  }

  // ノードkが保持する情報を子ノードから更新
  inline void _update_max_v(int k, ll *max_v, ll *smax_v, Pair *sum_mv, Pair *sum_v) {
    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      sum_mv[k] = sum_mv[2*k+2];
      sum_v[k] = max(max(sum_mv[2*k+1], sum_v[2*k+1]), sum_v[2*k+2]);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      sum_mv[k] = sum_mv[2*k+1];
      sum_v[k] = max(sum_v[2*k+1], max(sum_mv[2*k+2], sum_v[2*k+2]));
    } else {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      sum_mv[k] = max(sum_mv[2*k+1], sum_mv[2*k+2]);
      sum_v[k] = max(sum_v[2*k+1], sum_v[2*k+2]);
    }
  }

  // いずれも区間最大値でないペア(A_i, B_i)の中から(A_i+B_i)が最大となるもので更新する
  inline void update_sum_n(Pair &sn, int k, ll ma, ll mb) {
    if(sum_ma[k].a != ma && sum_ma[k].b != mb) {
      sn = max(sn, sum_ma[k]);
    }
    if(sum_a[k].a != ma && sum_a[k].b != mb) {
      sn = max(sn, sum_a[k]);
    }
    if(sum_mb[k].a != ma && sum_mb[k].b != mb) {
      sn = max(sn, sum_mb[k]);
    }
    if(sum_b[k].a != ma && sum_b[k].b != mb) {
      sn = max(sn, sum_b[k]);
    }
  }

  void update(int k) {
    // sum_ma[k], sum_a[k], sum_mb[k], sum_b[k] を計算する
    _update_max_v(k, max_va, smax_va, sum_ma, sum_a);
    _update_max_v(k, max_vb, smax_vb, sum_mb, sum_b);

    // 区間内のA_i, B_iの最大値が分かった時点で sum_n[k] を計算する
    sum_n[k] = max(sum_n[2*k+1], sum_n[2*k+2]);
    update_sum_n(sum_n[k], 2*k+1, max_va[k], max_vb[k]);
    update_sum_n(sum_n[k], 2*k+2, max_va[k], max_vb[k]);
  }

  void _add_val(ll xa, ll xb, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, xa, xb);
      return;
    }

    push(k);
    _add_val(xa, xb, a, b, 2*k+1, l, (l+r)/2);
    _add_val(xa, xb, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_min_a(ll xa, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_va[k] <= xa) {
      return;
    }
    if(a <= l && r <= b && smax_va[k] < xa) {
      update_node_max(k, xa, inf);
      return;
    }

    push(k);
    _update_min_a(xa, a, b, 2*k+1, l, (l+r)/2);
    _update_min_a(xa, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_min_b(ll xb, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_vb[k] <= xb) {
      return;
    }
    if(a <= l && r <= b && smax_vb[k] < xb) {
      update_node_max(k, inf, xb);
      return;
    }

    push(k);
    _update_min_b(xb, a, b, 2*k+1, l, (l+r)/2);
    _update_min_b(xb, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return max(
        max(max(sum_ma[k], sum_a[k]), max(sum_mb[k], sum_b[k])),
        sum_n[k]
      ).sum();
    }
    push(k);
    ll lv = _query_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_max(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

public:
  SegmentTree(int n, ll *a = nullptr, ll *b = nullptr) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<n; ++i) {
      ll va = (a != nullptr ? a[i] : 0);
      ll vb = (b != nullptr ? b[i] : 0);
      max_va[n0-1+i] = va;
      max_vb[n0-1+i] = vb;
      smax_va[n0-1+i] = smax_vb[n0-1+i] = -inf;

      sum_ma[n0-1+i] = sum_mb[n0-1+i] = Pair{va, vb};
      sum_a[n0-1+i] = sum_b[n0-1+i] = Pair{-inf, -inf};
      sum_n[n0-1+i] = Pair{-inf, -inf};
    }

    for(int i=n; i<n0; ++i) {
      max_va[n0-1+i] = smax_va[n0-1+i] = -inf;
      max_vb[n0-1+i] = smax_vb[n0-1+i] = -inf;

      sum_ma[n0-1+i] = sum_mb[n0-1+i] = Pair{-inf, -inf};
      sum_a[n0-1+i] = sum_b[n0-1+i] = Pair{-inf, -inf};
      sum_n[n0-1+i] = Pair{-inf, -inf};
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_min_a(int a, int b, ll x) {
    _update_min_a(x, a, b, 0, 0, n0);
  }

  void update_min_b(int a, int b, ll x) {
    _update_min_b(x, a, b, 0, 0, n0);
  }

  void add_val_a(int a, int b, ll x) {
    _add_val(x, 0, a, b, 0, 0, n0);
  }

  void add_val_b(int a, int b, ll x) {
    _add_val(0, x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  // デバッグ用
  void debug() {
    for(int i=0; i<2*n0-1; ++i) {
      printf("%d: max_va = %lld, smax_va = %lld, sum_ma = {%lld, %lld}, sum_a = {%lld, %lld}\n",
          i, max_va[i], smax_va[i], sum_ma[i].a, sum_ma[i].b, sum_a[i].a, sum_a[i].b
      );
      printf("%d: max_vb = %lld, smax_vb = %lld, sum_mb = {%lld, %lld}, sum_b = {%lld, %lld}\n",
          i, max_vb[i], smax_vb[i], sum_mb[i].a, sum_mb[i].b, sum_b[i].a, sum_b[i].b
      );
      printf("%d: sum_n = {%lld, %lld}, ladd = {%lld, %lld}\n", i, sum_n[i].a, sum_n[i].b, ladd_a[i], ladd_b[i]);
    }
  }
};

// ===== 実装ここまで =====


// 動作確認用クラス
class NaiveSolver {
  int n;
  ll a[N], b[N];

public:
  NaiveSolver(int n, ll *a = nullptr, ll *b = nullptr) : n(n) {
    for(int i=0; i<n; ++i) {
      this->a[i] = (a != nullptr ? a[i] : 0);
      this->b[i] = (b != nullptr ? b[i] : 0);
    }
  }

  void update_min_a(int l, int r, ll x) {
    for(int i=l; i<r; ++i) {
      if(x < a[i]) a[i] = x;
    }
  }
  void update_min_b(int l, int r, ll x) {
    for(int i=l; i<r; ++i) {
      if(x < b[i]) b[i] = x;
    }
  }
  void add_val_a(int l, int r, ll x) {
    for(int i=l; i<r; ++i) {
      a[i] += x;
    }
  }
  void add_val_b(int l, int r, ll x) {
    for(int i=l; i<r; ++i) {
      b[i] += x;
    }
  }
  ll query_max(int l, int r) {
    ll res = -1e18;
    for(int i=l; i<r; ++i) {
      if(res < a[i]+b[i]) res = a[i] + b[i];
    }
    return res;
  }

  void debug() {
    printf("A: ");
    for(int i=0; i<n; ++i) printf("%lld ", a[i]);
    printf("\n");

    printf("B: ");
    for(int i=0; i<n; ++i) printf("%lld ", b[i]);
    printf("\n");
  }
};

// 動作確認
ll v[N], w[N];
int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  uniform_int_distribution<ll> val(-1e5, 1e5);

  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  for(int i=0; i<n; ++i) w[i] = val(mt);

  SegmentTree stb(n, v, w);
  NaiveSolver checker(n, v, w);

  int a, b;
  ll cnt = 0;
  ll x, r0, r1;
  while(1) {
    a = gen(mt); b = gen(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(rtype(mt)) {
      case 0:
        // a<=i<b について A_i の値を min(A_i, x) に更新
        x = val(mt);
        stb.update_min_a(a, b, x);
        checker.update_min_a(a, b, x);
        break;
      case 1:
        // a<=i<b について B_i の値を min(B_i, x) に更新
        x = val(mt);
        stb.update_min_b(a, b, x);
        checker.update_min_b(a, b, x);
        break;
      case 2:
        // a<=i<b について A_i の値を A_i + x に更新
        x = val(mt);
        stb.add_val_a(a, b, x);
        checker.add_val_a(a, b, x);
        break;
      case 3:
        // a<=i<b について B_i の値を B_i + x に更新
        x = val(mt);
        stb.add_val_b(a, b, x);
        checker.add_val_b(a, b, x);
        break;
      case 4:
        // a<=i<b の中から A_i + B_i の最大値を求める
        r0 = stb.query_max(a, b);
        r1 = checker.query_max(a, b);
        if(r0 != r1) {
          printf("%lld: query max (%d, %d) : result = %lld (%c) expected = %lld\n", cnt, a, b, r0, (r0 < r1 ? '<' : '>'), r1);
          checker.debug();
          stb.debug();
        }
        break;
      default:
        continue;
    }
    ++cnt;
  }
  return 0;
}
