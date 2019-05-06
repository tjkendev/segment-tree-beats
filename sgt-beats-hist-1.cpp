#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Historic Information)
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の A_i の総和を求める
// - l<=i<r の中の B_i の最大値を求める
// - l<=i<r の中の B_i の総和を求める
// - (各クエリ後、全てのiについて B_i = max(A_i, B_i))

#define N 10003

class SegmentTree {
  const ll inf = 1e18;
  int n, n0;
  ll len[4*N];
  ll cur_s[4*N], cur_ma[4*N];
  ll ladd[4*N], ladd_m[4*N];

  // D_i = B_i - A_i の最小値管理
  ll min_v[4*N], smin_v[4*N];
  ll min_c[4*N], min_s[4*N];
  // D_iが同じものの中で、A_iが最大のものを管理する
  // D_i = 0の時の最大値更新のために使う
  ll min_vc[4*N];
  ll hmax_m[4*N];

  void update_node_min(int k, ll x) {
    //cout << "update_node_min " << k << " " << min_v[k] << " " << x << endl;
    min_s[k] += (x - min_v[k]) * min_c[k];
    min_v[k] = x;
  }

  void addall(int k, ll a, ll ma) {
    //cout << "addall " << k << " " << a << " " << ma << endl;
    //cout << " current " << ladd[k] << " " << ladd_m[k] << endl;
    cur_s[k] += a * len[k];
    cur_ma[k] += a;

    hmax_m[k] = max(hmax_m[k], min_vc[k] + ma);
    min_vc[k] += a;

    min_v[k] -= a; smin_v[k] -= a;
    min_s[k] -= a * len[k];

    ladd_m[k] = max(ladd_m[k], ladd[k] + ma);
    ladd[k] += a;
  }

  void push(int k) {
    if(k >= n0-1) return;
    //cout << "push down " << k << endl;

    if(ladd[k] != 0 || ladd_m[k] != 0) {
      addall(2*k+1, ladd[k], ladd_m[k]);
      addall(2*k+2, ladd[k], ladd_m[k]);
      ladd[k] = ladd_m[k] = 0;
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
    hmax_m[k] = max(hmax_m[2*k+1], hmax_m[2*k+2]);

    if(min_v[2*k+1] > min_v[2*k+2]) {
      min_v[k] = min_v[2*k+2];
      min_c[k] = min_c[2*k+2];
      smin_v[k] = min(min_v[2*k+1], smin_v[2*k+2]);

      min_vc[k] = min_vc[2*k+2];
    } else if(min_v[2*k+1] < min_v[2*k+2]) {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1];
      smin_v[k] = min(smin_v[2*k+1], min_v[2*k+2]);

      min_vc[k] = min_vc[2*k+1];
    } else {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1] + min_c[2*k+2];
      smin_v[k] = min(smin_v[2*k+1], smin_v[2*k+2]);

      min_vc[k] = max(min_vc[2*k+1], min_vc[2*k+2]);
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
      addall(k, x, x);
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

  ll _query_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return cur_s[k];
    }
    push(k);
    ll lv = _query_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }


  ll _query_hist_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return hmax_m[k];
    }
    push(k);
    ll lv = _query_hist_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hist_max(a, b, 2*k+2, (l+r)/2, r);
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
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<2*n0-1; ++i) {
      ladd[i] = ladd_m[i] = 0;
    }

    for(int i=0; i<n; ++i) {
      hmax_m[n0-1+i] = cur_s[n0-1+i] = min_vc[n0-1+i] = cur_ma[n0-1+i] = (a != nullptr ? a[i] : 0);
      min_v[n0-1+i] = min_s[n0-1+i] = 0;
      smin_v[n0-1+i] = inf;
      min_c[n0-1+i] = 1;
    }
    for(int i=n; i<n0; ++i) {
      hmax_m[n0-1+i] = cur_s[n0-1+i] = min_vc[n0-1+i] = cur_ma[n0-1+i] = 0;
      min_v[n0-1+i] = smin_v[n0-1+i] = inf;
      min_c[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  ll query_sum(int a, int b) {
    return _query_sum(a, b, 0, 0, n0);
  }

  ll query_hmax(int a, int b) {
    return _query_hist_max(a, b, 0, 0, n0);
  }

  ll query_hmax_sum(int a, int b) {
    return _query_hist_max_sum(a, b, 0, 0, n0);
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  for(int i=0; i<n; ++i) v[i] = w[i] = val(mt);
  for(int i=0; i<n; ++i) cout << v[i] << " ";
  cout << " ( ";
  for(int i=0; i<n; ++i) cout << w[i] << " ";
  cout << ")" << endl;

  SegmentTree stb(n, v);
  int a, b;
  ll x, r0, r1;
  int rest = 2000000;
  while(1) {

    a = gen(mt); b = gen(mt);
    x = val(mt);
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(rtype(mt)) {
      case 0:
        //if(rest == 0 || x == 0) continue;
        //--rest;
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
      case 3:
        r0 = stb.query_hmax(a, b);
        r1 = -1e18;
        for(int i=a; i<b; ++i) {
          r1 = max(r1, w[i]);
        }
        if(r0 != r1) {
          cout << "query hmax (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 4:
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
