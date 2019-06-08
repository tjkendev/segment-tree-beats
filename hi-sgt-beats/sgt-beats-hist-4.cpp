#include<algorithm>
#include<random>
#include<cassert>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Historic Information)
// - i<=i<r について A_i の値を max(A_i, x) に更新
// - i<=i<r について A_i の値を min(A_i, x) に更新
// - l<=i<r について A_i の値に x を加える
// - l<=i<r について A_i の値を x に更新
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の A_i の最小値を求める
// - l<=i<r の中の B_i の総和を求める
// - l<=i<r の中の C_i の総和を求める
// - (各クエリ後、全てのiについて B_i = max(A_i, B_i), C_i = min(A_i, C_i))

#define N 5003

class SegmentTree {
  const static ll inf = 1e18;

  struct HistVal {
    // historic maximal value
    ll min_v, smin_v, min_c, min_s;
    // historic minimal value
    ll max_v, smax_v, max_c, max_s;

    void init(ll x) {
      min_v = 0; smin_v = inf; min_c = 1;
      max_v = 0; smax_v = -inf; max_c = 1;
      min_s = max_s = 0;
    }

    void init_empty() {
      min_v = smin_v = inf; min_c = 0;
      max_v = smax_v = -inf; max_c = 0;
      min_s = max_s = 0;
    }

    inline void update_min(ll x) {
      if(min_v < x) {
        min_s += (x - min_v) * min_c;
        min_v = x;
      }
    }

    inline void update_max(ll x) {
      if(x < max_v) {
        max_s += (x - max_v) * max_c;
        max_v = x;
      }
    }

    void update(HistVal &v) {
      update_min(v.min_v);
      update_max(v.max_v);
    }

    void merge(HistVal &l, HistVal &r) {
      min_s = l.min_s + r.min_s;

      if(l.min_v < r.min_v) {
        smin_v = min(l.smin_v, r.min_v);
        min_v = l.min_v;
        min_c = l.min_c;
      } else if(l.min_v > r.min_v) {
        smin_v = min(l.min_v, r.smin_v);
        min_v = r.min_v;
        min_c = r.min_c;
      } else {
        smin_v = min(l.smin_v, r.smin_v);
        min_v = l.min_v;
        min_c = l.min_c + r.min_c;
      }

      max_s = l.max_s + r.max_s;

      if(l.max_v > r.max_v) {
        smax_v = max(l.smax_v, r.max_v);
        max_v = l.max_v;
        max_c = l.max_c;
      } else if(l.max_v < r.max_v) {
        smax_v = max(l.max_v, r.smax_v);
        max_v = r.max_v;
        max_c = r.max_c;
      } else {
        smax_v = max(l.smax_v, r.smax_v);
        max_v = l.max_v;
        max_c = l.max_c + r.max_c;
      }
    }
    void add(ll x, ll c) {
      if(min_v != inf) min_v += x;
      if(smin_v != inf) smin_v += x;
      min_s += x * c;

      if(max_v != -inf) max_v += x;
      if(smax_v != -inf) smax_v += x;
      max_s += x * c;
    }
  };

  int n, n0;
  ll len[4*N], ladd[4*N];

  HistVal max_d[4*N], nval_d[4*N], min_d[4*N];

  ll max_v[4*N], smax_v[4*N], max_c[4*N];
  ll min_v[4*N], smin_v[4*N], min_c[4*N];
  ll sum[4*N];

  void update_node_max(int k, ll x) {
    sum[k] += (x - max_v[k]) * max_c[k];

    max_d[k].add(max_v[k] - x, max_c[k]);

    if(max_v[k] == min_v[k]) {
      min_d[k].add(min_v[k] - x, min_c[k]);
      max_v[k] = min_v[k] = x;
    } else if(max_v[k] == smin_v[k]) {
      max_v[k] = smin_v[k] = x;
    } else {
      max_v[k] = x;
    }
  }

  void update_node_min(int k, ll x) {
    sum[k] += (x - min_v[k]) * min_c[k];

    min_d[k].add(min_v[k] - x, min_c[k]);

    if(min_v[k] == max_v[k]) {
      max_d[k].add(max_v[k] - x, max_c[k]);
      min_v[k] = max_v[k] = x;
    } else if(min_v[k] == smax_v[k]) {
      min_v[k] = smax_v[k] = x;
    } else {
      min_v[k] = x;
    }
  }

  void addall(int k, ll a) {
    //printf("addall %d %lld\n", k, a);
    sum[k] += a * len[k];
    max_v[k] += a;
    if(smax_v[k] != -inf) smax_v[k] += a;

    min_v[k] += a;
    if(smin_v[k] != inf) smin_v[k] += a;

    max_d[k].add(-a, max_c[k]);
    if(max_v[k] != min_v[k]) {
      nval_d[k].add(-a, len[k] - min_c[k] - max_c[k]);
    }
    min_d[k].add(-a, min_c[k]);

    ladd[k] += a;
    //printf("addall %d: max_d[k] = {%lld, %lld}, nval_d[k] = {%lld, %lld}\n", k, max_d[k].min_v, max_d[k].sv, nval_d[k].min_v, nval_d[k].sv);
  }

  void push(int k) {
    if(k >= n0-1) return;

    if(ladd[k] != 0) {
      addall(2*k+1, ladd[k]);
      addall(2*k+2, ladd[k]);
      ladd[k] = 0;
    }

    if(max_v[2*k+1] > max_v[k]) {
      update_node_max(2*k+1, max_v[k]);
    }
    if(max_v[2*k+2] > max_v[k]) {
      update_node_max(2*k+2, max_v[k]);
    }
    if(min_v[k] > min_v[2*k+1]) {
      update_node_min(2*k+1, min_v[k]);
    }
    if(min_v[k] > min_v[2*k+2]) {
      update_node_min(2*k+2, min_v[k]);
    }

    // 2*k+1
    if(min_v[k] == min_v[2*k+1]) {
      min_d[2*k+1].update(min_d[k]);
    } else if(max_v[k] == min_v[2*k+1]) {
      // min_v[2*k+1] == max_v[2*k+1] == max_v[k]
      min_d[2*k+1].update(max_d[k]);
    } else {
      min_d[2*k+1].update(nval_d[k]);
    }

    if(max_v[k] == max_v[2*k+1]) {
      max_d[2*k+1].update(max_d[k]);
    } else if(min_v[k] == max_v[2*k+1]) {
      // max_v[2*k+1] == min_v[2*k+1] == min_v[k]
      max_d[2*k+1].update(min_d[k]);
    } else {
      max_d[2*k+1].update(nval_d[k]);
    }

    nval_d[2*k+1].update(nval_d[k]);

    // 2*k+2
    if(min_v[k] == min_v[2*k+2]) {
      min_d[2*k+2].update(min_d[k]);
    } else if(max_v[k] == min_v[2*k+2]) {
      // min_v[2*k+2] == max_v[2*k+2] == max_v[k]
      min_d[2*k+2].update(max_d[k]);
    } else {
      min_d[2*k+2].update(nval_d[k]);
    }

    if(max_v[k] == max_v[2*k+2]) {
      max_d[2*k+2].update(max_d[k]);
    } else if(min_v[k] == max_v[2*k+2]) {
      // max_v[2*k+2] == min_v[2*k+2] == min_v[k]
      max_d[2*k+2].update(min_d[k]);
    } else {
      max_d[2*k+2].update(nval_d[k]);
    }

    nval_d[2*k+2].update(nval_d[k]);
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    nval_d[k].merge(nval_d[2*k+1], nval_d[2*k+2]);

    max_v[k] = max(max_v[2*k+1], max_v[2*k+2]);
    min_v[k] = min(min_v[2*k+1], min_v[2*k+2]);

    if(max_v[2*k+1] > max_v[2*k+2]) {
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      max_d[k] = max_d[2*k+1];
    } else if(max_v[2*k+1] < max_v[2*k+2]) {
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      max_d[k] = max_d[2*k+2];
    } else {
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      max_d[k].merge(max_d[2*k+1], max_d[2*k+2]);
    }

    if(min_v[2*k+1] < min_v[2*k+2]) {
      min_c[k] = min_c[2*k+1];
      smin_v[k] = min(smin_v[2*k+1], min_v[2*k+2]);

      min_d[k] = min_d[2*k+1];
    } else if(min_v[2*k+1] > min_v[2*k+2]) {
      min_c[k] = min_c[2*k+2];
      smin_v[k] = min(min_v[2*k+1], smin_v[2*k+2]);

      min_d[k] = min_d[2*k+2];
    } else {
      min_c[k] = min_c[2*k+1] + min_c[2*k+2];
      smin_v[k] = min(smin_v[2*k+1], smin_v[2*k+2]);

      min_d[k].merge(min_d[2*k+1], min_d[2*k+2]);
    }

    if(min_v[2*k+1] == max_v[2*k+1]) {
      if(min_v[k] < min_v[2*k+1] && max_v[2*k+1] < max_v[k]) {
        nval_d[k].merge(nval_d[k], max_d[2*k+1]);
      }
    } else { // min_v[2*k+1] < max_v[2*k+1]
      if(max_v[2*k+1] < max_v[k]) {
        nval_d[k].merge(nval_d[k], max_d[2*k+1]);
      }
      if(min_v[k] < min_v[2*k+1]) {
        nval_d[k].merge(nval_d[k], min_d[2*k+1]);
      }
    }

    if(min_v[2*k+2] == max_v[2*k+2]) {
      if(min_v[k] < min_v[2*k+2] && max_v[2*k+2] < max_v[k]) {
        nval_d[k].merge(nval_d[k], max_d[2*k+2]);
      }
    } else { // min_v[2*k+2] < max_v[2*k+2]
      if(max_v[2*k+2] < max_v[k]) {
        nval_d[k].merge(nval_d[k], max_d[2*k+2]);
      }
      if(min_v[k] < min_v[2*k+2]) {
        nval_d[k].merge(nval_d[k], min_d[2*k+2]);
      }
    }
  }

  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_v[k] <= x) {
      return;
    }
    if(a <= l && r <= b && smax_v[k] < x) {
      update_node_max(k, x);
      _update_dmax(k, l, r);
      _update_dmin(k, l, r);
      return;
    }

    push(k);
    _update_min(x, a, b, 2*k+1, l, (l+r)/2);
    _update_min(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_max(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || x <= min_v[k]) {
      return;
    }
    if(a <= l && r <= b && x < smin_v[k]) {
      update_node_min(k, x);
      _update_dmax(k, l, r);
      _update_dmin(k, l, r);
      return;
    }

    push(k);
    _update_max(x, a, b, 2*k+1, l, (l+r)/2);
    _update_max(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_dmax(int k, int l, int r) {
    if(l == r || (0 <= max_d[k].min_v && 0 <= nval_d[k].min_v && 0 <= min_d[k].min_v)) {
      return;
    }
    if(0 < max_d[k].smin_v && 0 < nval_d[k].smin_v && 0 < min_d[k].smin_v) {
      max_d[k].update_min(0);
      nval_d[k].update_min(0);
      min_d[k].update_min(0);
      return;
    }

    push(k);
    _update_dmax(2*k+1, l, (l+r)/2);
    _update_dmax(2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_dmin(int k, int l, int r) {
    if(l == r || (max_d[k].max_v <= 0 && nval_d[k].max_v <= 0 && min_d[k].max_v <= 0)) {
      return;
    }
    if(max_d[k].smax_v < 0 && nval_d[k].smax_v < 0 && min_d[k].smax_v < 0) {
      max_d[k].update_max(0);
      nval_d[k].update_max(0);
      min_d[k].update_max(0);
      return;
    }

    push(k);
    _update_dmin(2*k+1, l, (l+r)/2);
    _update_dmin(2*k+2, (l+r)/2, r);
    update(k);
  }

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      _update_dmax(k, l, r);
      _update_dmin(k, l, r);
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
      return max_v[k];
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
      return min_v[k];
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
      //printf("sum result: %d, %lld, %lld, %lld, %lld\n", k, sum[k], max_d[k].sum, nval_d[k].sum, min_d[k].sum);
      if(max_v[k] == min_v[k]) {
        return sum[k] + max_d[k].min_s;
      }
      return sum[k] + max_d[k].min_s + nval_d[k].min_s + min_d[k].min_s;
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
      //printf("sum result: %d, %lld, %lld, %lld, %lld\n", k, sum[k], max_d[k].sum, nval_d[k].sum, min_d[k].sum);
      if(max_v[k] == min_v[k]) {
        return sum[k] + max_d[k].max_s;
      }
      return sum[k] + max_d[k].max_s + nval_d[k].max_s + min_d[k].max_s;
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

    for(int i=0; i<2*n0-1; ++i) ladd[i] = 0;
    len[0] = n0;
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = min_v[n0-1+i] = sum[n0-1+i] = (a != nullptr ? a[i] : 0);
      smax_v[n0-1+i] = -inf; smin_v[n0-1+i] = inf;
      max_c[n0-1+i] = min_c[n0-1+i] = 1;

      max_d[n0-1+i].init(a[i]);
      nval_d[n0-1+i].init_empty();
      min_d[n0-1+i].init(a[i]);
    }
    for(int i=n; i<n0; ++i) {
      sum[n0-1+i] = 0;
      max_v[n0-1+i] = smax_v[n0-1+i] = -inf;
      min_v[n0-1+i] = smin_v[n0-1+i] = inf;
      max_c[n0-1+i] = min_c[n0-1+i] = 0;

      max_d[n0-1+i].init_empty();
      nval_d[n0-1+i].init_empty();
      min_d[n0-1+i].init_empty();
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  ll update_min(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
  }

  ll update_max(int a, int b, ll x) {
    _update_max(x, a, b, 0, 0, n0);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  void update_val(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
    _update_max(x, a, b, 0, 0, n0);
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

  void debug() {
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: ", k);
      printf("max = {%lld, %lld, %lld}, min = {%lld, %lld, %lld}, sum = %lld, Cmin = {%lld, %lld, %lld}, Cnmin = {%lld, %lld, %lld}, Cmmin = {%lld, %lld, %lld}, Csum = %lld, ladd = %lld\n",
          max_v[k], smax_v[k], max_c[k],
          min_v[k], smin_v[k], min_c[k], sum[k],
          max_d[k].min_v, max_d[k].smin_v, max_d[k].min_c,
          nval_d[k].min_v, nval_d[k].smin_v, nval_d[k].min_c,
          min_d[k].min_v, min_d[k].smin_v, min_d[k].min_c,
          max_d[k].min_s + nval_d[k].min_s + min_d[k].min_s, ladd[k]
          );
    }
  }
};

ll u[N], v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = 1000; //szrnd(mt);
  uniform_int_distribution<int> rtype(0, 7), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  const int limit = 2000;

  while(1) {
    printf("===============================\nbegin\n");
    for(int i=0; i<n; ++i) u[i] = v[i] = w[i] = val(mt);
    SegmentTree stb(n, v);

    //stb.debug();
    bool wrong = false;
    int a, b, t;
    ll x, r0, r1;
    int c = 1;
    while(c <= limit) {
      a = gen(mt); b = gen(mt);
      x = val(mt);
      if(a == b) continue;
      if(a > b) swap(a, b);
      t = rtype(mt);

      /*
      if(c == limit) {
        t = 2;
      } else {
        if(t == 2) continue;
      }
      //*/

      switch(t) {
        case 0:
          printf("%d: add_val (%d, %d) : %lld\n", c, a, b, x);
          stb.add_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] += x;
            w[i] = max(v[i], w[i]);
            u[i] = min(v[i], u[i]);
          }
          break;
        case 1:
          printf("%d: update_val (%d, %d) : %lld\n", c, a, b, x);
          stb.update_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = x;
            w[i] = max(v[i], w[i]);
            u[i] = min(v[i], u[i]);
          }
          break;
        case 2:
          printf("%d: query_max (%d, %d)\n", c, a, b);
          r0 = stb.query_max(a, b);
          r1 = -1e18;
          for(int i=a; i<b; ++i) {
            r1 = max(r1, v[i]);
          }
          if(r0 != r1) {
            printf("query max (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 3:
          printf("%d: query_min (%d, %d)\n", c, a, b);
          r0 = stb.query_max(a, b);
          r1 = -1e18;
          for(int i=a; i<b; ++i) {
            r1 = max(r1, v[i]);
          }
          if(r0 != r1) {
            printf("query max (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 4:
          printf("%d: query_hmax_sum (%d, %d)\n", c, a, b);
          r0 = stb.query_hmax_sum(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 += w[i];
          }
          if(r0 != r1) {
            printf("query hmax sum (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 5:
          printf("%d: query_hmin_sum (%d, %d)\n", c, a, b);
          r0 = stb.query_hmin_sum(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 += u[i];
          }
          if(r0 != r1) {
            printf("query hmin sum (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 6:
          printf("%d: update_min (%d, %d): %lld\n", c, a, b, x);
          stb.update_min(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = min(v[i], x);
            w[i] = max(v[i], w[i]);
            u[i] = min(v[i], u[i]);
          }
          break;
        case 7:
          printf("%d: update_max (%d, %d): %lld\n", c, a, b, x);
          stb.update_max(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = max(v[i], x);
            w[i] = max(w[i], v[i]);
            u[i] = min(v[i], u[i]);
          }
          break;
        default:
          continue;
      }
      //stb.debug();
      if(wrong) break;
      ++c;
    }
    if(wrong) break;
  }
  return 0;
}
