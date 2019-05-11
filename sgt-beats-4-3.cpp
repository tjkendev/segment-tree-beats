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
  const static ll inf = 1e18;
  struct Pair {
    ll a, b;
    ll sum() const { return a+b; }

    bool operator<(const Pair& other) const {
      return this->sum() < other.sum();
    }
  };

  int n, n0;
  Pair max_p[4*N][2][2];

  // AとBに関する最大値、二番目の最大値
  ll max_va[4*N], smax_va[4*N];
  ll max_vb[4*N], smax_vb[4*N];
  // 遅延させている加算値
  ll ladd_a[4*N], ladd_b[4*N];

  // ノードの各情報を更新する
  void update_node_max(int k, ll xa, ll xb) {
    if(xa < max_va[k]) {
      if(max_p[k][1][1].a != -inf) max_p[k][1][1].a = xa;
      if(max_p[k][1][0].a != -inf) max_p[k][1][0].a = xa;
      max_va[k] = xa;
    }

    if(xb < max_vb[k]) {
      if(max_p[k][1][1].b != -inf) max_p[k][1][1].b = xb;
      if(max_p[k][0][1].b != -inf) max_p[k][0][1].b = xb;
      max_vb[k] = xb;
    }
  }

  // 区間内のA_iにxaを加算し、区間内のB_iにxbを加算する
  inline void addall(int k, ll xa, ll xb) {
    if(xa != 0) {
      for(int i=0; i<2; ++i) for(int j=0; j<2; ++j) {
        if(max_p[k][i][j].a != -inf) max_p[k][i][j].a += xa;
      }

      max_va[k] += xa;
      if(smax_va[k] != -inf) smax_va[k] += xa;
      ladd_a[k] += xa;
    }

    if(xb != 0) {
      for(int i=0; i<2; ++i) for(int j=0; j<2; ++j) {
        if(max_p[k][i][j].b != -inf) max_p[k][i][j].b += xb;
      }

      max_vb[k] += xb;
      if(smax_vb[k] != -inf) smax_vb[k] += xb;
      ladd_b[k] += xb;
    }
  }

  // pushdown: 親ノードから子ノードへの伝搬
  void push(int k) {
    if(ladd_a[k] != 0 || ladd_b[k] != 0) {
      addall(2*k+1, ladd_a[k], ladd_b[k]);
      addall(2*k+2, ladd_a[k], ladd_b[k]);
      ladd_a[k] = ladd_b[k] = 0;
    }

    update_node_max(2*k+1, max_va[k], max_vb[k]);
    update_node_max(2*k+2, max_va[k], max_vb[k]);
  }

  inline void _update_max_v(int k, ll *max_v, ll *smax_v) {
    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);
    } else {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);
    }
  }

  // update: 子ノードから親ノードへの伝搬
  void update(int k) {
    _update_max_v(k, max_va, smax_va);
    _update_max_v(k, max_vb, smax_vb);

    for(int i=0; i<2; ++i) for(int j=0; j<2; ++j) {
      max_p[k][i][j] = Pair{-inf, -inf};
    }

    for(int i=0; i<2; ++i) for(int j=0; j<2; ++j) {
      Pair &p = max_p[2*k+1][i][j];
      Pair &e = max_p[k][p.a == max_va[k]][p.b == max_vb[k]];
      e = max(e, p);
    }
    for(int i=0; i<2; ++i) for(int j=0; j<2; ++j) {
      Pair &p = max_p[2*k+2][i][j];
      Pair &e = max_p[k][p.a == max_va[k]][p.b == max_vb[k]];
      e = max(e, p);
    }
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
      auto &mp = max_p[k];
      return max(max(mp[0][0], mp[0][1]), max(mp[1][0], mp[1][1])).sum();
    }
    push(k);
    ll lv = _query_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_max(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

public:
  SegmentTree(int n, ll *a, ll *b) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<n; ++i) {
      ll va = (a != nullptr ? a[i] : 0);
      ll vb = (b != nullptr ? b[i] : 0);
      max_va[n0-1+i] = va;
      max_vb[n0-1+i] = vb;
      smax_va[n0-1+i] = smax_vb[n0-1+i] = -inf;

      for(int p=0; p<2; ++p) for(int q=0; q<2; ++q) {
        max_p[n0-1+i][p][q] = Pair{-inf, -inf};
      }
      max_p[n0-1+i][1][1] = Pair{va, vb};
    }

    for(int i=n; i<n0; ++i) {
      max_va[n0-1+i] = smax_va[n0-1+i] = -inf;
      max_vb[n0-1+i] = smax_vb[n0-1+i] = -inf;

      for(int p=0; p<2; ++p) for(int q=0; q<2; ++q) {
        max_p[n0-1+i][p][q] = Pair{-inf, -inf};
      }
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
      auto &mp = max_p[i];
      printf("%d: max_va = %lld, smax_va = %lld, max_p00 = {%lld, %lld}, max_p01 = {%lld, %lld}\n",
          i, max_va[i], smax_va[i], mp[0][0].a, mp[0][0].b, mp[0][1].a, mp[0][1].b
      );
      printf("%d: max_vb = %lld, smax_vb = %lld, max_p10 = {%lld, %lld}, max_p11 = {%lld, %lld}\n",
          i, max_vb[i], smax_vb[i], mp[1][0].a, mp[1][0].b, mp[1][1].a, mp[1][1].b
      );
      printf("%d: ladd = {%lld, %lld}\n", i, ladd_a[i], ladd_b[i]);
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
  uniform_int_distribution<ll> val(-1e10, 1e10), val2(0, 1e10);

  int n = 16; //szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  for(int i=0; i<n; ++i) w[i] = val(mt);

  SegmentTree stb(n, v, w);
  NaiveSolver checker(n, v, w);

  //checker.debug();
  //stb.debug();

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
        x = val2(mt);
        stb.update_min_a(a, b, x);
        checker.update_min_a(a, b, x);
        break;
      case 1:
        // a<=i<b について B_i の値を min(B_i, x) に更新
        x = val2(mt);
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
    //checker.debug();
    //stb.debug();
  }
  return 0;
}
