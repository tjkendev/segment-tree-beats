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
  struct Val {
    ll a, b;
    ll sum() const { return a+b; }

    bool operator<(const Val& other) const {
      return this->sum() < other.sum();
    }
  };
  int n, n0;
  Val max_msa[4*N], max_sa[4*N];
  Val max_msb[4*N], max_sb[4*N];

  ll max_va[4*N], smax_va[4*N];
  ll max_vb[4*N], smax_vb[4*N];
  ll ladd_a[4*N], ladd_b[4*N];

  void update_node_max(int k, ll xa, ll xb) {
    if(xa < max_msb[k].a) {
      max_msb[k].a = xa;
    }
    if(xa < max_sb[k].a) {
      max_sb[k].a = xa;
    }
    if(xa < max_va[k]) {
      max_va[k] = max_msa[k].a = xa;
    }

    if(xb < max_msa[k].b) {
      max_msa[k].b = xb;
    }
    if(xb < max_sa[k].b) {
      max_sa[k].b = xb;
    }
    if(xb < max_vb[k]) {
      max_vb[k] = max_msb[k].b = xb;
    }
  }

  void addall(int k, ll xa, ll xb) {
    if(xa != 0) {
      max_msa[k].a += xa; max_sa[k].a += xa;
      max_msb[k].a += xa; max_sb[k].a += xa;

      max_va[k] += xa;
      if(smax_va[k] != -inf) smax_va[k] += xa;
      ladd_a[k] += xa;
    }

    if(xb != 0) {
      max_msa[k].b += xb; max_sa[k].b += xb;
      max_msb[k].b += xb; max_sb[k].b += xb;

      max_vb[k] += xb;
      if(smax_vb[k] != -inf) smax_vb[k] += xb;
      ladd_b[k] += xb;
    }
  }

  void push(int k) {
    if(ladd_a[k] != 0 || ladd_b[k] != 0) {
      addall(2*k+1, ladd_a[k], ladd_b[k]);
      addall(2*k+2, ladd_a[k], ladd_b[k]);
      ladd_a[k] = ladd_b[k] = 0;
    }

    update_node_max(2*k+1, max_va[k], max_vb[k]);
    update_node_max(2*k+2, max_va[k], max_vb[k]);
  }

  void _update_max_v(int k, ll *max_v, ll *smax_v, Val *max_mw, Val *max_w) {
    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      max_mw[k] = max_mw[2*k+2];
      max_w[k] = max(max(max_mw[2*k+1], max_w[2*k+1]), max_w[2*k+2]);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      max_mw[k] = max_mw[2*k+1];
      max_w[k] = max(max_w[2*k+1], max(max_mw[2*k+2], max_w[2*k+2]));
    } else {
      max_v[k] = max_v[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      max_mw[k] = max(max_mw[2*k+1], max_mw[2*k+2]);
      max_w[k] = max(max_w[2*k+1], max_w[2*k+2]);
    }
  }

  void update(int k) {
    _update_max_v(k, max_va, smax_va, max_msa, max_sa);
    _update_max_v(k, max_vb, smax_vb, max_msb, max_sb);
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
      return max(max(max_msa[k], max_sa[k]), max(max_msb[k], max_sb[k])).sum();
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

      max_msa[n0-1+i] = max_msb[n0-1+i] = Val{va, vb};
      max_sa[n0-1+i] = max_sb[n0-1+i] = Val{-inf, -inf};
    }

    for(int i=n; i<n0; ++i) {
      max_va[n0-1+i] = smax_va[n0-1+i] = -inf;
      max_vb[n0-1+i] = smax_vb[n0-1+i] = -inf;

      max_msa[n0-1+i] = max_msb[n0-1+i] = Val{-inf, -inf};
      max_sa[n0-1+i] = max_sb[n0-1+i] = Val{-inf, -inf};
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_min_a(int a, int b, ll x) {
    return _update_min_a(x, a, b, 0, 0, n0);
  }

  void update_min_b(int a, int b, ll x) {
    return _update_min_b(x, a, b, 0, 0, n0);
  }

  void add_val_a(int a, int b, ll x) {
    return _add_val(x, 0, a, b, 0, 0, n0);
  }

  void add_val_b(int a, int b, ll x) {
    return _add_val(0, x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);
  uniform_int_distribution<ll> val(0, 1e10);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  for(int i=0; i<n; ++i) w[i] = val(mt);
  SegmentTree stb(n, v, w);
  int a, b;
  ll x, r0, r1;
  while(1) {
    a = gen(mt); b = gen(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(rtype(mt)) {
      case 0:
        //cout << "update (" << a << ", " << b << ") = " << x << endl;
        x = val(mt);
        stb.update_min_a(a, b, x);
        for(int i=a; i<b; ++i) {
          if(x < v[i]) v[i] = x;
        }
        break;
      case 1:
        //cout << "update (" << a << ", " << b << ") = " << x << endl;
        x = val(mt);
        stb.update_min_b(a, b, x);
        for(int i=a; i<b; ++i) {
          if(x < w[i]) w[i] = x;
        }
        break;
      case 2:
        //cout << "update (" << a << ", " << b << ") = " << x << endl;
        x = val(mt);
        stb.add_val_a(a, b, x);
        for(int i=a; i<b; ++i) {
          v[i] += x;
        }
        break;
      case 3:
        //cout << "update (" << a << ", " << b << ") = " << x << endl;
        x = val(mt);
        stb.add_val_b(a, b, x);
        for(int i=a; i<b; ++i) {
          w[i] += x;
        }
        break;
      case 4:
        r0 = stb.query_max(a, b);
        r1 = -1e18;
        for(int i=a; i<b; ++i) {
          if(r1 < v[i] + w[i]) r1 = v[i] + w[i];
        }
        if(r0 != r1) {
          cout << "query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      default:
        continue;
    }
    //stb.debug();
  }
  return 0;
}
