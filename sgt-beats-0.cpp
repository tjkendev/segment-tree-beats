#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について、 A_i の値を min(A_i, x) に更新
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の A_i の和を求める

#define N 10003

class SegmentTreeBeats {
  const ll inf = 1e18;
  int n, n0;
  ll max_v[4*N], smax_v[4*N];
  ll sum[4*N], cnt[4*N];

  void push(int k) {
    if(max_v[k] < max_v[2*k+1]) {
      sum[2*k+1] += (max_v[k] - max_v[2*k+1]) * cnt[2*k+1];
      max_v[2*k+1] = max_v[k];
    }
    if(max_v[k] < max_v[2*k+2]) {
      sum[2*k+2] += (max_v[k] - max_v[2*k+2]) * cnt[2*k+2];
      max_v[2*k+2] = max_v[k];
    }
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      cnt[k] = cnt[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      cnt[k] = cnt[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);
    } else {
      max_v[k] = max_v[2*k+1];
      cnt[k] = cnt[2*k+1] + cnt[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);
    }
  }

  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_v[k] <= x) {
      return;
    }
    if(a <= l && r <= b && smax_v[k] < x) {
      sum[k] += (x - max_v[k]) * cnt[k];
      max_v[k] = x;
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

  ll _query_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum[k];
    }
    push(k);
    ll lv = _query_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

public:
  SegmentTreeBeats(int n) {
    SegmentTreeBeats(n, nullptr);
  }

  SegmentTreeBeats(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    if(a != nullptr) {
      for(int i=0; i<n; ++i) {
        max_v[n0-1+i] = sum[n0-1+i] = a[i];
        smax_v[n0-1+i] = -inf;
        cnt[n0-1+i] = 1;
      }
      for(int i=n; i<n0; ++i) {
        max_v[n0-1+i] = smax_v[n0-1+i] = 0;
        smax_v[n0-1+i] = -inf;
        cnt[n0-1+i] = 1;
      }
    } else {
      for(int i=n; i<n0; ++i) {
        max_v[n0-1+i] = smax_v[n0-1+i] = 0;
        smax_v[n0-1+i] = -inf;
        cnt[n0-1+i] = 0;
      }
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_min(int a, int b, ll x) {
    return _update_min(x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  ll query_sum(int a, int b) {
    return _query_sum(a, b, 0, 0, n0);
  }
};

ll v[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 3), gen(0, n);
  uniform_int_distribution<ll> val(0, 1e10);

  for(int i=0; i<n; ++i) v[i] = val(mt);
  SegmentTreeBeats stb(n, v);
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
        if(r0 != r1) {
          cout << "query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 2:
        r0 = stb.query_sum(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          r1 += v[i];
        }
        if(r0 != r1) {
          cout << "query sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
    }
  }
  return 0;
}
