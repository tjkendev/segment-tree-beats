#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

#define N 10003

// Segment Tree Beats (Historic Information)
// - i<=i<r について a_i の値を max(a_i, x) に更新
// - l<=i<r について a_i の値に x を加える
// - l<=i<r の中の a_i の最小値を求める
// - l<=i<r の中の c_i の最小値を求める
// - l<=i<r の中の c_i の総和を求める
// - (各クエリ後、全てのiについて c_i = min(a_i, c_i))

class SegmentTree {
  const static ll inf = 1e18;
  int n0;

  ll len[4*N], ladd[4*N];

  ll min_v[4*N], smin_v[4*N], min_c[4*N];
  ll sum[4*N];

  struct HistVal {
    ll max_v, smax_v, max_c, sum;
    ll m_hmin, nm_hmin;

    void init(ll x) {
      max_v = 0; smax_v = -inf; max_c = 1; sum = 0;
      m_hmin = x; nm_hmin = inf;
    }

    void init_empty() {
      max_v = smax_v = -inf; max_c = 0; sum = 0;
      m_hmin = nm_hmin = inf;
    }

    inline void update_max(ll x) {
      if(x < max_v) {
        sum += (x - max_v) * max_c;
        m_hmin += (x - max_v);

        max_v = x;
      }
    }

    inline void merge(HistVal &l, HistVal &r) {
      sum = l.sum + r.sum;
      nm_hmin = min(l.nm_hmin, r.nm_hmin);

      if(l.max_v > r.max_v) {
        smax_v = max(l.smax_v, r.max_v);
        max_v = l.max_v;
        max_c = l.max_c;

        nm_hmin = min(nm_hmin, r.m_hmin);
        m_hmin = l.m_hmin;
      } else if(l.max_v < r.max_v) {
        smax_v = max(l.max_v, r.smax_v);
        max_v = r.max_v;
        max_c = r.max_c;

        nm_hmin = min(nm_hmin, l.m_hmin);
        m_hmin = r.m_hmin;
      } else {
        max_v = l.max_v;
        smax_v = max(l.smax_v, r.smax_v);
        max_c = l.max_c + r.max_c;

        m_hmin = min(l.m_hmin, r.m_hmin);
      }
    }

    void add(ll x) {
      if(max_v != -inf) max_v += x;
      if(smax_v != -inf) smax_v += x;
    }

    ll hmin() const {
      return min(m_hmin, nm_hmin);
    }
  };

  HistVal min_d[4*N], nmin_d[4*N];

  void update_node_min(int k, ll x) {
    sum[k] += (x - min_v[k]) * min_c[k];

    min_d[k].add(min_v[k] - x);
    min_d[k].sum += (min_v[k] - x) * min_c[k];

    min_v[k] = x;
  }

  void addall(int k, ll a) {
    min_v[k] += a;
    if(smin_v[k] != inf) smin_v[k] += a;
    sum[k] += a * len[k];

    min_d[k].add(-a); nmin_d[k].add(-a);
    min_d[k].sum -= a * min_c[k];
    nmin_d[k].sum -= a * (len[k] - min_c[k]);

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

    if(min_v[2*k+1] < min_v[2*k+2]) {
      min_d[2*k+1].update_max(min_d[k].max_v);
      min_d[2*k+2].update_max(nmin_d[k].max_v);
    } else if(min_v[2*k+1] > min_v[2*k+2]) {
      min_d[2*k+1].update_max(nmin_d[k].max_v);
      min_d[2*k+2].update_max(min_d[k].max_v);
    } else {
      min_d[2*k+1].update_max(min_d[k].max_v);
      min_d[2*k+2].update_max(min_d[k].max_v);
    }

    nmin_d[2*k+1].update_max(nmin_d[k].max_v);
    nmin_d[2*k+2].update_max(nmin_d[k].max_v);
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    nmin_d[k].merge(nmin_d[2*k+1], nmin_d[2*k+2]);

    if(min_v[2*k+1] < min_v[2*k+2]) {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1];
      smin_v[k] = min(smin_v[2*k+1], min_v[2*k+2]);

      min_d[k] = min_d[2*k+1];
      nmin_d[k].merge(nmin_d[k], min_d[2*k+2]);
    } else if(min_v[2*k+1] > min_v[2*k+2]) {
      min_v[k] = min_v[2*k+2];
      min_c[k] = min_c[2*k+2];
      smin_v[k] = min(min_v[2*k+1], smin_v[2*k+2]);

      min_d[k] = min_d[2*k+2];
      nmin_d[k].merge(nmin_d[k], min_d[2*k+1]);
    } else {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1] + min_c[2*k+2];
      smin_v[k] = min(smin_v[2*k+1], smin_v[2*k+2]);

      min_d[k].merge(min_d[2*k+1], min_d[2*k+2]);
    }
  }

  void _update_dmin(int k, int l, int r) {
    if(l == r || (min_d[k].max_v <= 0 && nmin_d[k].max_v <= 0)) {
      return;
    }
    if(min_d[k].smax_v < 0 && nmin_d[k].smax_v < 0) {
      min_d[k].update_max(0);
      nmin_d[k].update_max(0);
      return;
    }

    push(k);
    _update_dmin(2*k+1, l, (l+r)/2);
    _update_dmin(2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_max(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || x <= min_v[k]) {
      return;
    }
    if(a <= l && r <= b && x < smin_v[k]) {
      update_node_min(k, x);
      _update_dmin(k, l, r);
      return;
    }

    push(k);
    _update_max(x, a, b, 2*k+1, l, (l+r)/2);
    _update_max(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      _update_dmin(k, l, r);
      return;
    }
    push(k);
    _add_val(x, a, b, 2*k+1, l, (l+r)/2);
    _add_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
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

  ll _query_hmin_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum[k] + min_d[k].sum + nmin_d[k].sum;
    }
    push(k);
    ll lv = _query_hmin_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmin_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

  ll _query_hmin_min(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return inf;
    }
    if(a <= l && r <= b) {
      return min(min_d[k].hmin(), nmin_d[k].hmin());
    }
    push(k);
    ll lv = _query_hmin_min(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmin_min(a, b, 2*k+2, (l+r)/2, r);
    return min(lv, rv);
  }

public:
  SegmentTree(int n, ll *a) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<2*n0-1; ++i) ladd[i] = 0;

    len[0] = n0;
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      min_v[n0-1+i] = sum[n0-1+i] = a[i];
      smin_v[n0-1+i] = inf;
      min_c[n0-1+i] = 1;

      min_d[n0-1+i].init(a[i]);
      nmin_d[n0-1+i].init_empty();
    }
    for(int i=n; i<n0; ++i) {
      sum[n0-1+i] = min_v[n0-1+i] = 0;
      smin_v[n0-1+i] = inf;
      min_c[n0-1+i] = 0;

      min_d[n0-1+i].init_empty();
      nmin_d[n0-1+i].init_empty();
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_max(int a, int b, ll x) {
    _update_max(x, a, b, 0, 0, n0);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  ll query_hmin_sum(int a, int b) {
    return _query_hmin_sum(a, b, 0, 0, n0);
  }

  ll query_hmin_min(int a, int b) {
    return _query_hmin_min(a, b, 0, 0, n0);
  }

  ll query_min(int a, int b) {
    return _query_min(a, b, 0, 0, n0);
  }

  void debug() {
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: ", k);
      printf("val = {%lld, %lld, %lld}, sum = %lld, Cmin = {%lld, %lld, %lld}, Cnmin = {%lld, %lld, %lld}, Csum = %lld, ladd = %lld\n",
          min_v[k], smin_v[k], min_c[k], sum[k],
          min_d[k].max_v, min_d[k].smax_v, min_d[k].max_c,
          nmin_d[k].max_v, nmin_d[k].smax_v, nmin_d[k].max_c, min_d[k].sum + nmin_d[k].sum,
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
  uniform_int_distribution<ll> val(-1e1, 1e1);

  const int limit = 200000;

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
            w[i] = min(v[i], w[i]);
          }
          break;
        case 1:
          printf("%d: query_min (%d, %d)\n", c, a, b);
          r0 = stb.query_min(a, b);
          r1 = 1e18;
          for(int i=a; i<b; ++i) {
            r1 = min(r1, v[i]);
          }
          if(r0 != r1) {
            printf("query max (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 2:
          printf("%d: query_hmin_sum (%d, %d)\n", c, a, b);
          r0 = stb.query_hmin_sum(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 += w[i];
          }
          if(r0 != r1) {
            printf("query hmin sum (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 3:
          printf("%d: update_max (%d, %d) : %lld\n", c, a, b, x);
          stb.update_max(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = max(v[i], x);
            w[i] = min(w[i], v[i]);
          }
          break;
        case 4:
          printf("%d: query_hmin_min (%d, %d)\n", c, a, b);
          r0 = stb.query_hmin_min(a, b);
          r1 = 1e18;
          for(int i=a; i<b; ++i) {
            r1 = min(r1, w[i]);
          }
          if(r0 != r1) {
            printf("query hmin min (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        default:
          continue;
      }
      if(wrong) break;
      //stb.debug();
      ++c;
    }
    if(c <= limit) {
      for(int i=0; i<n; ++i) cout << v[i] << " "; cout << endl;
      for(int i=0; i<n; ++i) cout << w[i] << " "; cout << endl;
      break;
    }
  }
  return 0;
}
