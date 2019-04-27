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
  Val max_sn[4*N];

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
      max_msa[k].a += xa;
      if(max_sa[k].a != -inf) max_sa[k].a += xa;
      max_msb[k].a += xa;
      if(max_sb[k].a != -inf) max_sb[k].a += xa;

      if(max_sn[k].a != -inf) max_sn[k].a += xa;

      max_va[k] += xa;
      if(smax_va[k] != -inf) smax_va[k] += xa;
      ladd_a[k] += xa;
    }

    if(xb != 0) {
      max_msa[k].b += xb;
      if(max_sa[k].b != -inf) max_sa[k].b += xb;
      max_msb[k].b += xb;
      if(max_sb[k].b != -inf) max_sb[k].b += xb;

      if(max_sn[k].b != -inf) max_sn[k].b += xb;

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

  inline void update_max_sn(Val &sn, int k, ll ma, ll mb) {
    if(max_msa[k].a != ma && max_msa[k].b != mb) {
      sn = max(sn, max_msa[k]);
    }
    if(max_sa[k].a != ma && max_sa[k].b != mb) {
      sn = max(sn, max_sa[k]);
    }
    if(max_msb[k].a != ma && max_msb[k].b != mb) {
      sn = max(sn, max_msb[k]);
    }
    if(max_sb[k].a != ma && max_sb[k].b != mb) {
      sn = max(sn, max_sb[k]);
    }
  }

  void update(int k) {
    _update_max_v(k, max_va, smax_va, max_msa, max_sa);
    _update_max_v(k, max_vb, smax_vb, max_msb, max_sb);

    max_sn[k] = max(max_sn[2*k+1], max_sn[2*k+2]);
    update_max_sn(max_sn[k], 2*k+1, max_va[k], max_vb[k]);
    update_max_sn(max_sn[k], 2*k+2, max_va[k], max_vb[k]);
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
      return max(max(max(max_msa[k], max_sa[k]), max(max_msb[k], max_sb[k])), max_sn[k]).sum();
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
      max_sn[n0-1+i] = Val{-inf, -inf};
    }

    for(int i=n; i<n0; ++i) {
      max_va[n0-1+i] = smax_va[n0-1+i] = -inf;
      max_vb[n0-1+i] = smax_vb[n0-1+i] = -inf;

      max_msa[n0-1+i] = max_msb[n0-1+i] = Val{-inf, -inf};
      max_sa[n0-1+i] = max_sb[n0-1+i] = Val{-inf, -inf};
      max_sn[n0-1+i] = Val{-inf, -inf};
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

  void debug() {
    for(int i=0; i<2*n0-1; ++i) {
      printf("%d: max_va = %lld, smax_va = %lld, max_msa = {%lld, %lld}, max_sa = {%lld, %lld}\n",
          i, max_va[i], smax_va[i], max_msa[i].a, max_msa[i].b, max_sa[i].a, max_sa[i].b
      );
      printf("%d: max_vb = %lld, smax_vb = %lld, max_msb = {%lld, %lld}, max_sb = {%lld, %lld}\n",
          i, max_vb[i], smax_vb[i], max_msb[i].a, max_msb[i].b, max_sb[i].a, max_sb[i].b
      );
      printf("%d: max_sn = {%lld, %lld}, ladd = {%lld, %lld}\n", i, max_sn[i].a, max_sn[i].b, ladd_a[i], ladd_b[i]);
    }
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);
  uniform_int_distribution<ll> val(-1e5, 1e5);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  for(int i=0; i<n; ++i) w[i] = val(mt);
  SegmentTree stb(n, v, w);
  int a, b;
  ll cnt = 0;
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
          printf("%lld: query max (%d, %d) : result = %lld (%c) expected = %lld\n", cnt, a, b, r0, (r0 < r1 ? '<' : '>'), r1);
          printf("A: "); for(int i=0; i<n; ++i) printf("%lld ", v[i]); printf("\n");
          printf("B: "); for(int i=0; i<n; ++i) printf("%lld ", w[i]); printf("\n");
          stb.debug();
        }
        break;
      default:
        continue;
    }
    ++cnt;
    //stb.debug();
  }
  return 0;
}
