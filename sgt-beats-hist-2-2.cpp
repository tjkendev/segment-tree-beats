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
// - l<=i<r の中の B_i の最大値を求める
// - (各クエリ後、全てのiについて B_i = max(A_i, B_i))

#define N 10003

class SegmentTree {
  const static ll inf = 1e18;
  int n0;
  ll len[4*N], ladd[4*N];

  ll max_v[4*N], smax_v[4*N], max_c[4*N];
  ll sum[4*N];

  struct MinVal {
    ll min_v, smin_v, min_c, sum;
    ll m_hmax, nm_hmax;

    void init(ll x) {
      min_v = 0; smin_v = inf; min_c = 1; sum = 0;
      m_hmax = x; nm_hmax = -inf;
    }

    void init_empty() {
      min_v = smin_v = inf; min_c = 0; sum = 0;
      m_hmax = nm_hmax = -inf;
    }

    inline void update_min(ll x) {
      if(min_v < x) {
        sum += (x - min_v) * min_c;
        m_hmax += (x - min_v);

        min_v = x;
      }
    }

    inline void merge(MinVal &l, MinVal &r) {
      sum = l.sum + r.sum;
      nm_hmax = max(l.nm_hmax, r.nm_hmax);

      if(l.min_v < r.min_v) {
        smin_v = min(l.smin_v, r.min_v);
        min_v = l.min_v;
        min_c = l.min_c;

        nm_hmax = max(nm_hmax, r.m_hmax);
        m_hmax = l.m_hmax;
      } else if(l.min_v > r.min_v) {
        smin_v = min(l.min_v, r.smin_v);
        min_v = r.min_v;
        min_c = r.min_c;

        nm_hmax = max(nm_hmax, l.m_hmax);
        m_hmax = r.m_hmax;
      } else {
        min_v = l.min_v;
        smin_v = min(l.smin_v, r.smin_v);
        min_c = l.min_c + r.min_c;

        m_hmax = max(l.m_hmax, r.m_hmax);
      }
    }

    void add(ll x) {
      if(min_v != inf) min_v += x;
      if(smin_v != inf) smin_v += x;
    }

    ll hmax() const {
      return max(m_hmax, nm_hmax);
    }
  };

  MinVal min_d[4*N], nmin_d[4*N];

  void update_node_max(int k, ll x) {
    sum[k] += (x - max_v[k]) * max_c[k];

    min_d[k].add(max_v[k] - x);
    min_d[k].sum += (max_v[k] - x) * max_c[k];

    max_v[k] = x;
  }

  void addall(int k, ll a) {
    max_v[k] += a;
    if(smax_v[k] != -inf) smax_v[k] += a;
    sum[k] += a * len[k];

    min_d[k].add(-a); nmin_d[k].add(-a);
    min_d[k].sum -= a * max_c[k];
    nmin_d[k].sum -= a * (len[k] - max_c[k]);

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
      min_d[2*k+1].update_min(nmin_d[k].min_v);
      min_d[2*k+2].update_min(min_d[k].min_v);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      min_d[2*k+1].update_min(min_d[k].min_v);
      min_d[2*k+2].update_min(nmin_d[k].min_v);
    } else {
      min_d[2*k+1].update_min(min_d[k].min_v);
      min_d[2*k+2].update_min(min_d[k].min_v);
    }

    nmin_d[2*k+1].update_min(nmin_d[k].min_v);
    nmin_d[2*k+2].update_min(nmin_d[k].min_v);
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    nmin_d[k].merge(nmin_d[2*k+1], nmin_d[2*k+2]);

    if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      min_d[k] = min_d[2*k+1];
      nmin_d[k].merge(nmin_d[k], min_d[2*k+2]);
    } else if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      min_d[k] = min_d[2*k+2];
      nmin_d[k].merge(nmin_d[k], min_d[2*k+1]);
    } else {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      min_d[k].merge(min_d[2*k+1], min_d[2*k+2]);
    }
  }

  // 数列Dの更新: d_i <- max(d_i, 0)
  void _update_dmax(int k, int l, int r) {
    if(l == r || (0 <= min_d[k].min_v && 0 <= nmin_d[k].min_v)) {
      return;
    }
    if(0 < min_d[k].smin_v && 0 < nmin_d[k].smin_v) {
      min_d[k].update_min(0);
      nmin_d[k].update_min(0);
      return;
    }

    push(k);
    _update_dmax(2*k+1, l, (l+r)/2);
    _update_dmax(2*k+2, (l+r)/2, r);
    update(k);
  }

  // 数列Aの更新: a_i <- min(a_i, x)
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

  // 数列Aの更新: a_i <- a_i + x
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

  // 数列Aのクエリ処理: max(a_i)
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

  // 数列A,Dのクエリ処理: sum(a_i + d_i)
  ll _query_hist_max_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum[k] + min_d[k].sum + nmin_d[k].sum;
    }
    push(k);
    ll lv = _query_hist_max_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hist_max_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

  // 数列A,Dのクエリ処理: max(a_i + d_i)
  ll _query_hist_max_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return max(min_d[k].hmax(), nmin_d[k].hmax());
    }
    push(k);
    ll lv = _query_hist_max_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hist_max_max(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

public:
  SegmentTree(int n, ll *a) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    len[0] = n0;
    for(int i=0; i<2*n0-1; ++i) {
      ladd[i] = 0;
      len[2*i+1] = len[2*i+2] = (len[i] >> 1);
    }

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = sum[n0-1+i] = a[i];
      smax_v[n0-1+i] = -inf;
      max_c[n0-1+i] = 1;

      min_d[n0-1+i].init(a[i]);
      nmin_d[n0-1+i].init_empty();
    }
    for(int i=n; i<n0; ++i) {
      sum[n0-1+i] = max_v[n0-1+i] = 0;
      smax_v[n0-1+i] = -inf;
      max_c[n0-1+i] = 0;

      min_d[n0-1+i].init_empty();
      nmin_d[n0-1+i].init_empty();
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_min(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  ll query_hmax_sum(int a, int b) {
    return _query_hist_max_sum(a, b, 0, 0, n0);
  }

  ll query_hmax_max(int a, int b) {
    return _query_hist_max_max(a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  void debug() {
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: ", k);
      printf("val = {%lld, %lld, %lld}, sum = %lld, Cmin = {%lld, %lld, %lld}, Cnmin = {%lld, %lld, %lld}, Csum = %lld, ladd = %lld\n",
          max_v[k], smax_v[k], max_c[k], sum[k],
          min_d[k].min_v, min_d[k].smin_v, min_d[k].min_c,
          nmin_d[k].min_v, nmin_d[k].smin_v, nmin_d[k].min_c, min_d[k].sum + nmin_d[k].sum,
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
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  const int limit = 300000;

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
        case 4:
          printf("%d: query_hmax_max (%d, %d)\n", c, a, b);
          r0 = stb.query_hmax_max(a, b);
          r1 = -1e18;
          for(int i=a; i<b; ++i) {
            r1 = max(r1, w[i]);
          }
          if(r0 != r1) {
            printf("query hmax max (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
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
