#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について、 a_i の値を min(a_i, x) に更新
// - l<=i<r の中の a_i の最大値を求める
// - l<=i<r の a_i の和を求める

// 区間総和を a_i が最大値の時・非最大値の時で分割したバージョン

#define N 10003

class SegmentTree {
  const ll inf = 1e18;
  int n, n0;
  ll max_v[4*N], smax_v[4*N], max_c[4*N];

  ll ladd[4*N]; // 区間加算クエリにおけるlazy tag
  ll len[4*N];  // ノードの区間に含まれる要素の数

  ll sum_a[4*N]; // 最大値の総和
  ll sum_b[4*N]; // 非最大値の総和

  void update_node_max(int k, ll x) {
    // 最大値の総和だけ加算
    sum_a[k] += (x - max_v[k]) * max_c[k];

    max_v[k] = x;
  }

  void addall(int k, ll x) {
    max_v[k] += x; smax_v[k] += x;

    // 最大値の総和・非最大値の総和それぞれに加算
    sum_a[k] += x * max_c[k];
    sum_b[k] += x * (len[k] - max_c[k]);

    ladd[k] += x;
  }

  void push(int k) {
    if(ladd[k]) {
      addall(2*k+1, ladd[k]);
      addall(2*k+2, ladd[k]);
      ladd[k] = 0;
    }

    if(max_v[k] < max_v[2*k+1]) {
      update_node_max(2*k+1, max_v[k]);
    }
    if(max_v[k] < max_v[2*k+2]) {
      update_node_max(2*k+2, max_v[k]);
    }
  }

  void update(int k) {
    sum_b[k] = sum_b[2*k+1] + sum_b[2*k+2];

    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      sum_a[k] = sum_a[2*k+2];
      sum_b[k] += sum_a[2*k+1];
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      sum_a[k] = sum_a[2*k+1];
      sum_b[k] += sum_a[2*k+2];
    } else {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      sum_a[k] = sum_a[2*k+1] + sum_a[2*k+2];
    }
  }

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
      // 最大値の総和・非最大値の総和を合わせる
      return sum_a[k] + sum_b[k];
    }
    push(k);
    ll lv = _query_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

public:
  SegmentTree(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    len[0] = n0;
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = sum_a[n0-1+i] = a[i];
      smax_v[n0-1+i] = -inf; sum_b[n0-1+i] = 0;
      max_c[n0-1+i] = 1;
    }

    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i] = smax_v[n0-1+i] = -inf;
      sum_a[n0-1+i] = sum_b[n0-1+i] = max_c[n0-1+i] = 0;
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
  SegmentTree stb(n, v);
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
