#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// 通常のSegment Tree with Lazy Propagation
// - l<=i<r について、 A_i の値を min(A_i, x) に更新
// - l<=i<r の中の A_i の最大値を求める

#define N 10003

class SegmentTree {
  const ll inf = 1e18;
  int n, n0;
  ll max_v[4*N];

  void push(int k) {
    if(max_v[k] < max_v[2*k+1]) {
      max_v[2*k+1] = max_v[k];
    }
    if(max_v[k] < max_v[2*k+2]) {
      max_v[2*k+2] = max_v[k];
    }
  }

  void update(int k) {
    max_v[k] = max(max_v[2*k+1], max_v[2*k+2]);
  }

  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      max_v[k] = min(max_v[k], x);
      return;
    }

    push(k);
    _update_min(x, a, b, 2*k+1, l, (l+r)/2);
    _update_min(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return max_v[k];
    }
    push(k);
    ll lv = _query_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_max(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

public:
  SegmentTree(int n) {
    SegmentTree(n, nullptr);
  }

  SegmentTree(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = (a != nullptr ? a[i] : 0);
    }

    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i] = -inf;
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_min(int a, int b, ll x) {
    return _update_min(x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }
};

ll v[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 1), gen(0, n);
  uniform_int_distribution<ll> val(0, 1e10);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  SegmentTree stb(n, v);
  int a, b;
  ll x, r0, r1;
  while(1) {
    a = gen(mt); b = gen(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(rtype(mt)) {
      case 0:
        cout << "update (" << a << ", " << b << ") = " << x << endl;
        x = val(mt);
        stb.update_min(a, b, x);
        for(int i=a; i<b; ++i) {
          if(x < v[i]) v[i] = x;
        }
        break;
      case 1:
        r0 = stb.query_max(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          if(r1 < v[i]) r1 = v[i];
        }
        //if(r0 != r1) {
          cout << "query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        //}
        break;
    }
  }
  return 0;
}
