#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Historic Information)
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の B_i の総和を求める
// - (各クエリ後、全てのiについて B_i = max(A_i, B_i))

#define N 10003

class SegmentTree {
  const ll inf = 1e18;
  int n, n0;
  ll len[4*N];
  ll cur_v[4*N], cur_s[4*N];
  ll cur_ma[4*N];
  ll ladd[4*N];

  ll min_v[4*N], smin_v[4*N];
  ll min_c[4*N], min_s[4*N];


  void update_node_min(int k, ll x) {
    min_s[k] += (x - min_v[k]) * min_c[k];
    min_v[k] = x;
  }

  void addall(int k, ll a) {
    cur_s[k] += a * len[k];
    cur_ma[k] += a;

    min_v[k] -= a; smin_v[k] -= a;
    min_s[k] -= a * len[k];
    ladd[k] += a;
  }

  void push(int k) {
    if(k >= n0-1) return;

    if(ladd[k] != 0) {
      addall(2*k+1, ladd[k]);
      addall(2*k+2, ladd[k]);
      ladd[k] = 0;
    }

    if(min_v[2*k+1] < min_v[k]) {
      update_node_min(2*k+1, min_v[k]);
    }
    if(min_v[2*k+2] < min_v[k]) {
      update_node_min(2*k+2, min_v[k]);
    }
  }

  void update(int k) {
    cur_s[k] = cur_s[2*k+1] + cur_s[2*k+2];
    cur_ma[k] = max(cur_ma[2*k+1], cur_ma[2*k+2]);
    min_s[k] = min_s[2*k+1] + min_s[2*k+2];

    if(min_v[2*k+1] > min_v[2*k+2]) {
      min_v[k] = min_v[2*k+2];
      min_c[k] = min_c[2*k+2];
      smin_v[k] = min(min_v[2*k+1], smin_v[2*k+2]);
    } else if(min_v[2*k+1] < min_v[2*k+2]) {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1];
      smin_v[k] = min(smin_v[2*k+1], min_v[2*k+2]);
    } else {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1] + min_c[2*k+2];
      smin_v[k] = min(smin_v[2*k+1], smin_v[2*k+2]);
    }
  }

  void _update_max(ll x, int k, int l, int r) {
    if(l == r || x <= min_v[k]) {
      return;
    }
    if(x < smin_v[k]) {
      update_node_min(k, x);
      return;
    }

    push(k);
    _update_max(x, 2*k+1, l, (l+r)/2);
    _update_max(x, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      _update_max(0, k, l, r);
      return;
    }
    push(k);
    _add_val(x, a, b, 2*k+1, l, (l+r)/2);
    _add_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return cur_ma[k];
    }
    push(k);
    ll lv = _query_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_max(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

  ll _query_hist_max_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return cur_s[k] + min_s[k];
    }
    push(k);
    ll lv = _query_hist_max_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hist_max_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

public:
  SegmentTree(int n, ll *a = nullptr) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    len[0] = n0;
    for(int i=0; i<2*n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      cur_ma[n0-1+i] = cur_v[n0-1+i] = cur_s[n0-1+i] = (a != nullptr ? a[i] : 0);
      min_v[n0-1+i] = 0;
      smin_v[n0-1+i] = inf;
      min_c[n0-1+i] = 1;
    }
    for(int i=n; i<n0; ++i) {
      cur_v[n0-1+i] = cur_s[n0-1+i] = cur_ma[n0-1+i] = 0;
      min_v[n0-1+i] = smin_v[n0-1+i] = inf;
      min_c[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  ll query_hmax_sum(int a, int b) {
    return _query_hist_max_sum(a, b, 0, 0, n0);
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
  uniform_int_distribution<int> rtype(0, 2), gen(0, n);
  uniform_int_distribution<ll> val(-1e4, 1e4);

  for(int i=0; i<n; ++i) v[i] = w[i] = val(mt);

  SegmentTree stb(n, v);
  int a, b;
  ll x, r0, r1;
  while(1) {
    a = gen(mt); b = gen(mt);
    x = val(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(rtype(mt)) {
      case 0:
        //cout << "add_val (" << a << ", " << b << ") : " << x << endl;
        stb.add_val(a, b, x);
        for(int i=a; i<b; ++i) {
          v[i] += x;
          w[i] = max(v[i], w[i]);
        }
        break;
      case 1:
        r0 = stb.query_max(a, b);
        r1 = -1e18;
        for(int i=a; i<b; ++i) {
          r1 = max(r1, v[i]);
        }
        if(r0 != r1) {
          cout << "query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
      case 2:
        r0 = stb.query_hmax_sum(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          r1 += w[i];
        }
        if(r0 != r1) {
          cout << "query hmax sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
    }
  }
  return 0;
}
