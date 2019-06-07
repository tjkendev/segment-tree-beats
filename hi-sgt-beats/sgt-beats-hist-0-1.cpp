#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Historic Information)
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の B_i の総和を求める
// - l<=i<r の中の C_i の総和を求める
// - (各クエリ後、全てのiについて B_i = max(A_i, B_i))
// - (各クエリ後、全てのiについて C_i = min(A_i, C_i))

#define N 10003

class SegmentTree {
  const static ll inf = 1e18;

  struct HMinVal {
    ll min_v, smin_v, min_c, sum;

    HMinVal() {
      min_v = smin_v = inf;
      min_c = sum = 0;
    }

    void init() {
      min_v = 0;
      min_c = 1;
    }

    void update_min(ll x) {
      if(min_v < x) {
        sum += (x - min_v) * min_c;
        min_v = x;
      }
    }

    void add(ll a, ll len) {
      min_v += a;
      if(smin_v != inf) smin_v += a;

      sum += a * len;
    }

    void merge(HMinVal &l, HMinVal &r) {
      sum = l.sum + r.sum;

      if(l.min_v > r.min_v) {
        min_v = r.min_v;
        min_c = r.min_c;
        smin_v = min(l.min_v, r.smin_v);
      } else if(l.min_v < r.min_v) {
        min_v = l.min_v;
        min_c = l.min_c;
        smin_v = min(l.smin_v, r.min_v);
      } else { // l.min_v == r.min_v
        min_v = l.min_v;
        min_c = l.min_c + r.min_c;
        smin_v = min(l.smin_v, r.smin_v);
      }
    }
  };


  int n, n0;
  ll len[4*N];
  ll cur_v[4*N], cur_s[4*N];
  ll cur_ma[4*N], cur_mi[4*N];
  ll ladd[4*N];

  HMinVal hmax[4*N], hmin[4*N];

  void addall(int k, ll a) {
    cur_s[k] += a * len[k];
    cur_ma[k] += a;
    cur_mi[k] += a;

    hmax[k].add(-a, len[k]);
    hmin[k].add(a, len[k]);

    ladd[k] += a;
  }

  void push(int k) {
    if(k >= n0-1) return;

    if(ladd[k] != 0) {
      addall(2*k+1, ladd[k]);
      addall(2*k+2, ladd[k]);
      ladd[k] = 0;
    }

    hmax[2*k+1].update_min(hmax[k].min_v);
    hmax[2*k+2].update_min(hmax[k].min_v);

    hmin[2*k+1].update_min(hmin[k].min_v);
    hmin[2*k+2].update_min(hmin[k].min_v);
  }

  void update(int k) {
    cur_s[k] = cur_s[2*k+1] + cur_s[2*k+2];
    cur_ma[k] = max(cur_ma[2*k+1], cur_ma[2*k+2]);
    cur_mi[k] = min(cur_mi[2*k+1], cur_mi[2*k+2]);

    hmax[k].merge(hmax[2*k+1], hmax[2*k+2]);
    hmin[k].merge(hmin[2*k+1], hmin[2*k+2]);
  }

  void _update_hmax(int k, int l, int r) {
    if(l == r || 0 <= hmax[k].min_v) {
      return;
    }
    if(0 < hmax[k].smin_v) {
      hmax[k].update_min(0);
      return;
    }

    push(k);
    _update_hmax(2*k+1, l, (l+r)/2);
    _update_hmax(2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_hmin(int k, int l, int r) {
    if(l == r || 0 <= hmin[k].min_v) {
      return;
    }
    if(0 < hmin[k].smin_v) {
      hmin[k].update_min(0);
      return;
    }

    push(k);
    _update_hmin(2*k+1, l, (l+r)/2);
    _update_hmin(2*k+2, (l+r)/2, r);
    update(k);
  }

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      _update_hmax(k, l, r);
      _update_hmin(k, l, r);
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

  ll _query_min(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return inf;
    }
    if(a <= l && r <= b) {
      return cur_mi[k];
    }
    push(k);
    ll lv = _query_min(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_min(a, b, 2*k+2, (l+r)/2, r);
    return min(lv, rv);
  }

  ll _query_hist_max_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return cur_s[k] + hmax[k].sum;
    }
    push(k);
    ll lv = _query_hist_max_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hist_max_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

  ll _query_hist_min_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return cur_s[k] - hmin[k].sum;
    }
    push(k);
    ll lv = _query_hist_min_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hist_min_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

public:
  SegmentTree(int n, ll *a = nullptr) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    len[0] = n0;
    for(int i=0; i<2*n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      cur_ma[n0-1+i] = cur_mi[n0-1+i] = cur_v[n0-1+i] = cur_s[n0-1+i] = (a != nullptr ? a[i] : 0);
      hmax[n0-1+i].init();
      hmin[n0-1+i].init();
    }
    for(int i=n; i<n0; ++i) {
      cur_v[n0-1+i] = cur_s[n0-1+i] = 0;
      cur_ma[n0-1+i] = -inf; cur_mi[n0-1+i] = inf;
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  ll query_hmax_sum(int a, int b) {
    return _query_hist_max_sum(a, b, 0, 0, n0);
  }

  ll query_hmin_sum(int a, int b) {
    return _query_hist_min_sum(a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  ll query_min(int a, int b) {
    return _query_min(a, b, 0, 0, n0);
  }
};

ll u[N], v[N], w[N], s[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);
  uniform_int_distribution<ll> val(-1e4, 1e4);

  for(int i=0; i<n; ++i) u[i] = v[i] = w[i] = val(mt);

  SegmentTree stb(n, v);
  int a, b;
  ll x, r0, r1;
  int t = 0;
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
          u[i] = min(v[i], u[i]);
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
        r0 = stb.query_min(a, b);
        r1 = 1e18;
        for(int i=a; i<b; ++i) {
          r1 = min(r1, v[i]);
        }
        if(r0 != r1) {
          cout << "query min (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 3:
        r0 = stb.query_hmax_sum(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          r1 += w[i];
        }
        if(r0 != r1) {
          cout << "query hmax sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 4:
        r0 = stb.query_hmin_sum(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          r1 += u[i];
        }
        if(r0 != r1) {
          cout << "query hmin sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      default:
        continue;
    }
    ++t;
  }
  return 0;
}
