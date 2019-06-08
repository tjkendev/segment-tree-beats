#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Histric Information)
// - i<=i<r について、 A_i の値を max(A_i, x) に更新
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の B_i の総和を求める
// - (各クエリ後、全てのiについて B_i = max(A_i, B_i))

#define N 10003

class SegmentTree {
  const static ll inf = 1e18;
  int n, n0;
  ll len[4*N];
  ll ladd[4*N];

  struct MinVal {
    ll v, sv, c;

    void merge2(MinVal &l, MinVal &r) {
      if(l.v < r.v) {
        v = l.v;
        sv = min(l.sv, r.v);
        c = l.c;
      } else if(l.v > r.v) {
        v = r.v;
        sv = min(l.v, r.sv);
        c = r.c;
      } else {
        v = l.v;
        sv = min(l.sv, r.sv);
        c = l.c + r.c;
      }
    }
    void merge(MinVal &mv) {
      if(v < mv.v) {
        sv = min(sv, mv.v);
      } else if(v > mv.v) {
        sv = min(v, mv.sv);
        v = mv.v;
        c = mv.c;
      } else {
        sv = min(sv, mv.sv);
        c += mv.c;
      }
    }
    void add(ll x) {
      if(v != inf) v += x;
      if(sv != inf) sv += x;
    }
  };

  MinVal dmin[4*N], dmin2[4*N];
  ll dmin_s[4*N];

  ll max_v[4*N], smax_v[4*N];
  ll max_c[4*N], sum[4*N];

  void update_node_dmin(int k, ll x) {
    dmin_s[k] += (x - dmin[k].v) * dmin[k].c;
    dmin[k].v = x;
  }

  void update_node_dmin2(int k, ll x) {
    dmin_s[k] += (x - dmin2[k].v) * dmin2[k].c;
    dmin2[k].v = x;
  }

  void update_node_max(int k, ll x) {
    sum[k] += (x - max_v[k]) * max_c[k];

    dmin[k].add(max_v[k] - x);
    dmin_s[k] += (max_v[k] - x) * max_c[k];

    max_v[k] = x;
  }

  void addall(int k, ll a) {
    sum[k] += a * len[k];
    max_v[k] += a;
    if(smax_v[k] != -inf) smax_v[k] += a;

    dmin[k].add(-a); dmin2[k].add(-a);
    dmin_s[k] -= a * len[k];
    ladd[k] += a;
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

    if(max_v[2*k+1] < max_v[2*k+2]) {
      if(dmin[2*k+2].v < dmin[k].v) {
        update_node_dmin(2*k+2, dmin[k].v);
      }
      if(dmin[2*k+1].v < dmin2[k].v) {
        update_node_dmin(2*k+1, dmin2[k].v);
      }
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      if(dmin[2*k+2].v < dmin2[k].v) {
        update_node_dmin(2*k+2, dmin2[k].v);
      }
      if(dmin[2*k+1].v < dmin[k].v) {
        update_node_dmin(2*k+1, dmin[k].v);
      }
    } else {
      if(dmin[2*k+1].v < dmin[k].v) {
        update_node_dmin(2*k+1, dmin[k].v);
      }
      if(dmin[2*k+2].v < dmin[k].v) {
        update_node_dmin(2*k+2, dmin[k].v);
      }
    }

    if(dmin2[2*k+1].v < dmin2[k].v) {
      update_node_dmin2(2*k+1, dmin2[k].v);
    }
    if(dmin2[2*k+2].v < dmin2[k].v) {
      update_node_dmin2(2*k+2, dmin2[k].v);
    }
  }

  void update(int k) {
    dmin_s[k] = dmin_s[2*k+1] + dmin_s[2*k+2];
    sum[k] = sum[2*k+1] + sum[2*k+2];

    dmin2[k].merge2(dmin2[2*k+1], dmin2[2*k+2]);

    if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      dmin[k] = dmin[2*k+1];
      dmin2[k].merge(dmin[2*k+2]);
    } else if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      dmin[k] = dmin[2*k+2];
      dmin2[k].merge(dmin[2*k+1]);
    } else {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      dmin[k].merge2(dmin[2*k+1], dmin[2*k+2]);
    }
  }

  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_v[k] <= x) {
      return;
    }
    if(a <= l && r <= b && smax_v[k] < x) {
      update_node_max(k, x);
      _update_dmax(k, l, r);
      return;
    }

    push(k);
    _update_min(x, a, b, 2*k+1, l, (l+r)/2);
    _update_min(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_dmax(int k, int l, int r) {
    if(l == r || (0 <= dmin[k].v && 0 <= dmin2[k].v)) {
      return;
    }
    if(0 < dmin[k].sv && 0 < dmin2[k].sv) {
      if(0 < dmin[k].sv && dmin[k].v < 0) {
        update_node_dmin(k, 0);
      }
      if(0 < dmin2[k].sv && dmin2[k].v < 0) {
        update_node_dmin2(k, 0);
      }
      return;
    }

    push(k);
    _update_dmax(2*k+1, l, (l+r)/2);
    _update_dmax(2*k+2, (l+r)/2, r);
    update(k);
  }

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      _update_dmax(k, l, r);
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

  ll _query_hist_max_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum[k] + dmin_s[k];
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
    for(int i=0; i<2*n0-1; ++i) {
      ladd[i] = 0;
      len[2*i+1] = len[2*i+2] = (len[i] >> 1);
    }

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = sum[n0-1+i] = (a != nullptr ? a[i] : 0);
      smax_v[n0-1+i] = -inf;
      max_c[n0-1+i] = 1;
      dmin[n0-1+i].v = 0; dmin[n0-1+i].sv = inf; dmin[n0-1+i].c = 1;
      dmin2[n0-1+i].v = dmin2[n0-1+i].sv = inf; dmin2[n0-1+i].c = 0;
      dmin_s[n0-1+i] = 0;
    }
    for(int i=n; i<n0; ++i) {
      sum[n0-1+i] = max_v[n0-1+i] = 0;
      smax_v[n0-1+i] = -inf;
      max_c[n0-1+i] = 0;
      dmin[n0-1+i].v = dmin[n0-1+i].sv = inf; dmin[n0-1+i].c = 0;
      dmin2[n0-1+i].v = dmin2[n0-1+i].sv = inf; dmin2[n0-1+i].c = 0;
      dmin_s[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  ll update_min(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
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

  void debug() {
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: ", k);
      printf("val = {%lld, %lld, %lld}, sum = %lld, Cmin = {%lld, %lld, %lld}, Cnmin = {%lld, %lld, %lld}, Csum = %lld, ladd = %lld\n",
          max_v[k], smax_v[k], max_c[k], sum[k],
          dmin[k].v, dmin[k].sv, dmin[k].c,
          dmin2[k].v, dmin2[k].sv, dmin2[k].c, dmin_s[k],
          ladd[k]
          );
    }
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = 128; //szrnd(mt);
  uniform_int_distribution<int> rtype(0, 3), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  const int limit = 30000;

  while(1) {
    printf("===============================\nbegin\n");
    for(int i=0; i<n; ++i) v[i] = w[i] = val(mt);
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

      switch(t) {
        case 0:
          printf("%d: add_val (%d, %d) : %lld\n", c, a, b, x);
          stb.add_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] += x;
            w[i] = max(v[i], w[i]);
          }
          break;
        case 1:
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
        case 2:
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
        case 3:
          printf("%d: update_min (%d, %d)\n", c, a, b);
          stb.update_min(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = min(v[i], x);
          }
          break;
        default:
          continue;
      }
      //stb.debug();
      if(wrong) break;
      ++c;
    }
    if(c <= limit) break;
  }
  return 0;
}
