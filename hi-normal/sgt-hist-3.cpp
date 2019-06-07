#include<iostream>
#include<random>
using namespace std;
using ll = long long;

// Segment Tree with Lazy Propagation
// l<=i<r について a_i を a_i + x に更新
// l<=i<r の s_i の区間総和を求める

#define N 10004

class SegmentTree {
  ll n0;
  ll va[4*N], vb[4*N];

  ll ladd_a[4*N], ladd_b[4*N];
  ll len[4*N];

  void addall(int k, ll x, ll s) {
    va[k] += x * len[k];
    vb[k] += s * len[k];

    ladd_a[k] += x;
    ladd_b[k] += s;
  }

  void push(int k) {
    addall(2*k+1, ladd_a[k], ladd_b[k]);
    addall(2*k+2, ladd_a[k], ladd_b[k]);
    ladd_a[k] = ladd_b[k] = 0;
  }

  void update(int k) {
    va[k] = va[2*k+1] + va[2*k+2];
    vb[k] = vb[2*k+1] + vb[2*k+2];
  }

  void _add_val(ll x, ll t, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x, -t*x);
      return;
    }

    push(k);
    _add_val(x, t, a, b, 2*k+1, l, (l+r)/2);
    _add_val(x, t, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_sum(ll t, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return va[k] * t + vb[k];
    }
    push(k);
    ll lv = _query_sum(t, a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum(t, a, b, 2*k+2, (l+r)/2, r);
    update(k);
    return lv + rv;
  }

public:
  SegmentTree(int n, ll *a) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    len[0] = n0;
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      va[n0-1+i] = vb[n0-1+i] = a[i];
    }
    for(int i=n; i<n0; ++i) {
      va[n0-1+i] = vb[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; --i) update(i);
  }

  void add_val(int a, int b, ll x, ll t) {
    _add_val(x, t, a, b, 0, 0, n0);
  }

  ll query_sum(int a, int b, ll t) {
    return _query_sum(t, a, b, 0, 0, n0);
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(1000, 10000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 1), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  for(int i=0; i<n; ++i) v[i] = w[i] = val(mt);
  SegmentTree stb(n, v);
  int a, b;
  ll x, r0, r1;
  int c = 0;
  ll t0 = 0;
  bool show = false;
  while(++c) {
    a = gen(mt); b = gen(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(rtype(mt)) {
      case 0:
        x = val(mt);
        if(show) {
          cout << "add val (" << a << ", " << b << ", " << t0 << ") = " << x << endl;
        }
        stb.add_val(a, b, x, t0);
        for(int i=a; i<b; ++i) {
          v[i] += x;
        }
        break;
      case 1:
        if(show) {
          cout << "query sum (" << a << ", " << b << ", " << t0 << ") " << endl;
        }
        r0 = stb.query_sum(a, b, t0);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          r1 += w[i];
        }
        if(r0 != r1) {
          cout << "(wrong) query sum (" << a << ", " << b << ", " << t0 << ") " << r0 << " " << r1 << endl;
        }
        break;
      default:
        continue;
    }
    t0++;
    for(int i=0; i<n; ++i) {
      w[i] += v[i];
    }
  }
  return 0;
}


