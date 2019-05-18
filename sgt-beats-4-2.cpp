#include<algorithm>
#include<random>
#include<iostream>
#include<cassert>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について A_i の値を min(A_i, x) に更新
// - l<=i<r について B_i の値を x に更新する
// - l<=i<r の中の A_i + B_i の最大値を求める

#define N 10003

class SegmentTree {
  const static ll inf = 1e18;

  struct Pair {
    ll a, b;

    ll sum() const { return a + b; }

    // Pair同士を比較する時は a_i + b_i の値が大きい方を大きいとする
    bool operator<(const Pair &other) const {
      return this->sum() < other.sum();
    }
  };
  int n, n0;
  // a_i の最大値、二番目の最大値
  ll max_v[4*N], smax_v[4*N];

  // 遅延させている b_i の更新値
  ll lval[4*N];

  // a_i が最大値となる中で a_i + b_i が最大値となるペア P_A = (a_i, b_i)
  Pair max_p[4*N];
  // a_i が非最大値となる中で a_i + b_i が最大値となるペア P_B = (a_i, b_i)
  Pair nmax_p[4*N];

  void update_node_max(int k, ll x) {
    // max_p[k].a の値のみを更新する
    // nmax_p[k].a の値は最大値未満であるため更新する必要なし
    max_p[k].a = x;

    max_v[k] = x;
  }

  void update_val(int k, ll x) {
    // ペアに含まれる b_i の値を x に更新
    max_p[k].b = x;
    if(nmax_p[k].b != -inf) nmax_p[k].b = x;

    lval[k] = x;
  }

  // 親ノードから子ノードに情報を伝搬
  void push(int k) {
    // a_i の更新
    if(max_v[k] < max_v[2*k+1]) {
      update_node_max(2*k+1, max_v[k]);
    }
    if(max_v[k] < max_v[2*k+2]) {
      update_node_max(2*k+2, max_v[k]);
    }

    // b_i の更新
    if(lval[k] != inf) {
      update_val(2*k+1, lval[k]);
      update_val(2*k+2, lval[k]);
      lval[k] = inf;
    }
  }

  // 子ノードから親ノードの情報を更新
  void update(int k) {
    // 2*k+1, 2*k+2 の非最大値のうちの最大値を 非最大値として更新
    nmax_p[k] = max(nmax_p[2*k+1], nmax_p[2*k+2]);

    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      // 2*k+2 は最大値、2*k+1 は非最大値 として更新
      max_p[k] = max_p[2*k+2];
      nmax_p[k] = max(nmax_p[k], max_p[2*k+1]);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      // 2*k+1 は最大値、2*k+2 は非最大値 として更新
      max_p[k] = max_p[2*k+1];
      nmax_p[k] = max(nmax_p[k], max_p[2*k+2]);
    } else {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      // 2*k+1, 2*k+2 共に最大値 として更新
      max_p[k] = max(max_p[2*k+1], max_p[2*k+2]);
    }
  }

  // 区間[a, b)に含まれる a_i の値を min(a_i, x) に更新
  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_v[k] <= x) {
      return;
    }
    if(a <= l && r <= b && smax_v[k] < x) {
      update_node_max(k, x);
      return;
    }

    push(k);
    _update_min(x, a, b, 2*k+1, l, (l+r)/2);
    _update_min(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  // 区間[a, b)に含まれる b_i の値を x に更新
  void _update_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      update_val(k, x);
      return;
    }

    push(k);
    _update_val(x, a, b, 2*k+1, l, (l+r)/2);
    _update_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  // 区間[a, b)の中の a_i + b_i の最大値を求める
  ll _query_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return max(max_p[k], nmax_p[k]).sum();
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

    for(int i=0; i<2*n0-1; ++i) lval[i] = inf;

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = a[i];
      smax_v[n0-1+i] = -inf;

      // [i:i+1)の最大値ペアは (a_i, b_i)
      max_p[n0-1+i] = Pair{a[i], b[i]};
      // [i:i+1)の2番目の最大値ペアは (-∞, -∞)
      nmax_p[n0-1+i] = Pair{-inf, -inf};
    }
    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i] = smax_v[n0-1+i] = -inf;
      max_p[n0-1+i] = nmax_p[n0-1+i] = Pair{-inf, -inf};
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  // 区間[a, b) について a_i の値を min(a_i, x) に更新
  void update_min(int a, int b, ll x) {
    return _update_min(x, a, b, 0, 0, n0);
  }

  // 区間[a, b) について b_i の値を x に更新
  void update_val(int a, int b, ll x) {
    _update_val(x, a, b, 0, 0, n0);
  }

  // 区間[a, b) の中で max(a_i + b_i) を求める
  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(1000, 10000);
  int n = 16; //szrnd(mt);
  uniform_int_distribution<int> rtype(0, 2), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  for(int i=0; i<n; ++i) w[i] = val(mt);
  SegmentTree stb(n, v, w);
  int a, b;
  ll x, r0, r1;
  int c = 0;
  bool show = false;
  while(++c) {
    a = gen(mt); b = gen(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    x = val(mt);

    switch(rtype(mt)) {
      case 0:
        if(show) {
          cout << "update min (" << a << ", " << b << ") = " << x << endl;
        }
        stb.update_min(a, b, x);
        for(int i=a; i<b; ++i) {
          if(x < v[i]) v[i] = x;
        }
        break;
      case 1:
        if(show) {
          cout << "update val (" << a << ", " << b << ") = " << x << endl;
        }
        stb.update_val(a, b, x);
        for(int i=a; i<b; ++i) {
          w[i] = x;
        }
        break;
      case 2:
        r0 = stb.query_max(a, b);
        r1 = (-1e18);
        for(int i=a; i<b; ++i) {
          if(r1 < v[i] + w[i]) r1 = v[i] + w[i];
        }
        if(show || r0 != r1) {
          cout << "query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      default:
        continue;
    }
  }
  return 0;
}
