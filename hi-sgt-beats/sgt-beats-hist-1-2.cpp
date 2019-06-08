#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Historic Information)
// - i<=i<r について a_i の値を max(a_i, x) に更新
// - l<=i<r について a_i の値に x を加える
// - l<=i<r の中の a_i の最小値を求める
// - l<=i<r の中の b_i の総和を求める
// - l<=i<r の中の b_i の最大値を求める
// - (各クエリ後、全てのiについて b_i = max(a_i, b_i))

#define N 10003

class SegmentTree {
  const static ll inf = 1e18;
  int n0;

  // 区間加算クエリ用
  ll len[4*N], ladd[4*N];

  // a_i の最小値・二番目の最小値・最小値の個数
  ll min_v[4*N], smin_v[4*N], min_c[4*N];
  // a_i の区間総和
  ll sum[4*N];

  struct HistVal {
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

    inline void merge(HistVal &l, HistVal &r) {
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

  //  min_d: a_i が最小値の i における d_i まわりの情報
  // nmin_d: a_i が非最小値の i における d_i まわりの情報
  HistVal min_d[4*N], nmin_d[4*N];


  // a_i の最小値を x に更新する
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

    // a_i の最小値に d_i の最小値情報を伝搬
    if(min_v[2*k+1] < min_v[2*k+2]) {
      min_d[2*k+1].update_min(min_d[k].min_v);
      min_d[2*k+2].update_min(nmin_d[k].min_v);
    } else if(min_v[2*k+1] > min_v[2*k+2]) {
      min_d[2*k+1].update_min(nmin_d[k].min_v);
      min_d[2*k+2].update_min(min_d[k].min_v);
    } else {
      min_d[2*k+1].update_min(min_d[k].min_v);
      min_d[2*k+2].update_min(min_d[k].min_v);
    }

    // a_i の非最小値に d_i の最小値情報を伝搬
    nmin_d[2*k+1].update_min(nmin_d[k].min_v);
    nmin_d[2*k+2].update_min(nmin_d[k].min_v);
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

  // (内部用) d_i <- max(d_i, 0) で更新
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

  // 区間[a, b) の a_i を max(a_i, x) で更新
  void _update_max(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || x <= min_v[k]) {
      return;
    }
    if(a <= l && r <= b && x < smin_v[k]) {
      update_node_min(k, x);
      _update_dmax(k, l, r);
      return;
    }

    push(k);
    _update_max(x, a, b, 2*k+1, l, (l+r)/2);
    _update_max(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  // 区間[a, b) の a_i に x を加算する
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

  // 区間[a, b) における a_i の区間最小値を返す
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

  // 区間[a, b) における historic maximal value の区間総和を返す
  ll _query_hmax_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum[k] + min_d[k].sum + nmin_d[k].sum;
    }
    push(k);
    ll lv = _query_hmax_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmax_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

  // 区間[a, b) における historic maximal value の区間最大値を返す
  ll _query_hmax_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return max(min_d[k].hmax(), nmin_d[k].hmax());
    }
    push(k);
    ll lv = _query_hmax_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmax_max(a, b, 2*k+2, (l+r)/2, r);
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

  // i<=i<r について a_i の値を max(a_i, x) に更新
  void update_max(int a, int b, ll x) {
    _update_max(x, a, b, 0, 0, n0);
  }

  // l<=i<r について a_i の値に x を加える
  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  // l<=i<r の中の b_i の区間総和を求める
  ll query_hmax_sum(int a, int b) {
    return _query_hmax_sum(a, b, 0, 0, n0);
  }

  // l<=i<r の中の b_i の区間最大値を求める
  ll query_hmax_max(int a, int b) {
    return _query_hmax_max(a, b, 0, 0, n0);
  }

  // l<=i<r の中の a_i の区間最小値を求める
  ll query_min(int a, int b) {
    return _query_min(a, b, 0, 0, n0);
  }

  void debug() {
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: ", k);
      printf("val = {%lld, %lld, %lld}, sum = %lld, Cmin = {%lld, %lld, %lld}, Cnmin = {%lld, %lld, %lld}, Csum = %lld, ladd = %lld\n",
          min_v[k], smin_v[k], min_c[k], sum[k],
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
          printf("%d: update_max (%d, %d) : %lld\n", c, a, b, x);
          stb.update_max(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = max(v[i], x);
            w[i] = max(w[i], v[i]);
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
      if(wrong) break;
      //stb.debug();
      ++c;
    }
    if(c <= limit) {
      for(int i=0; i<n; ++i) cout << v[i] << " "; cout << endl;
      for(int i=0; i<n; ++i) cout << w[i] << " "; cout << endl;
      stb.debug();
      break;
    }
  }
  return 0;
}
