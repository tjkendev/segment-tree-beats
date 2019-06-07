#include<algorithm>
#include<random>
#include<cassert>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats (Historic Information)
// - i<=i<r について a_i の値を max(a_i, x) に更新
// - i<=i<r について a_i の値を min(a_i, x) に更新
// - l<=i<r について a_i の値に x を加える
// - l<=i<r について a_i の値を x に更新
// - l<=i<r の中の b_i の最大値を求める
// - l<=i<r の中の b_i の総和を求める
// - (各クエリ後、全てのiについて b_i = max(a_i, b_i))

#define N 5003

class SegmentTree {
  const static ll inf = 1e18;

  struct HistVal {
    ll min_v, smin_v, min_c, min_s;
    ll m_hmax, nm_hmax;

    // a_i = x でノードを初期化
    void init(ll x) {
      min_v = 0; smin_v = inf; min_c = 1;
      min_s = 0;
      m_hmax = x; nm_hmax = -inf;
    }

    // 要素なしでノードを初期化
    void init_empty() {
      min_v = smin_v = inf; min_c = 0;
      min_s = 0;
      m_hmax = nm_hmax = -inf;
    }

    inline void update_min(ll x) {
      if(min_v < x) {
        min_s += (x - min_v) * min_c;
        m_hmax += (x - min_v);

        min_v = x;
      }
    }

    // l と r の情報をマージして更新
    void merge(HistVal &l, HistVal &r) {
      min_s = l.min_s + r.min_s;
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
        smin_v = min(l.smin_v, r.smin_v);
        min_v = l.min_v;
        min_c = l.min_c + r.min_c;

        m_hmax = max(l.m_hmax, r.m_hmax);
      }
    }

    // 要素c個にxを加算
    void add(ll x, ll c) {
      if(min_v != inf) min_v += x;
      if(smin_v != inf) smin_v += x;
      min_s += x * c;
    }

    // 過去最大値を返す
    ll hmax() const {
      return max(m_hmax, nm_hmax);
    }
  };

  int n0;
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
  }

  // ノードk のhistoric informationを ノードt へ伝搬
  void _push_hist(int t, int k) {
    // a_i が最小値の d_i の最小値に対する伝搬
    if(min_v[k] == min_v[t]) {
      min_d[t].update_min(min_d[k].min_v);
    } else if(max_v[k] == min_v[t]) {
      // min_v[t] == max_v[t] == max_v[k]
      min_d[t].update_min(max_d[k].min_v);
    } else {
      min_d[t].update_min(nval_d[k].min_v);
    }

    // a_i が最大値の d_i の最小値に対する伝搬
    if(max_v[k] == max_v[t]) {
      max_d[t].update_min(max_d[k].min_v);
    } else if(min_v[k] == max_v[t]) {
      // max_v[t] == min_v[t] == min_v[k]
      max_d[t].update_min(min_d[k].min_v);
    } else {
      max_d[t].update_min(nval_d[k].min_v);
    }

    // a_i が非最大値・非最小値の d_i の最小値に対する伝搬
    nval_d[t].update_min(nval_d[k].min_v);
  }

  void push(int k) {
    if(k >= n0-1) return;

    if(ladd[k] != 0) {
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
    if(min_v[2*k+1] < min_v[k]) {
      update_node_min(2*k+1, min_v[k]);
    }
    if(min_v[2*k+2] < min_v[k]) {
      update_node_min(2*k+2, min_v[k]);
    }

    _push_hist(2*k+1, k);
    _push_hist(2*k+2, k);
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    nval_d[k].merge(nval_d[2*k+1], nval_d[2*k+2]);

    // a_i の最大値情報のマージ
    if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      max_d[k] = max_d[2*k+1];
    } else if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      max_d[k] = max_d[2*k+2];
    } else {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      max_d[k].merge(max_d[2*k+1], max_d[2*k+2]);
    }

    // a_i の最小値情報のマージ
    if(min_v[2*k+1] < min_v[2*k+2]) {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1];
      smin_v[k] = min(smin_v[2*k+1], min_v[2*k+2]);

      min_d[k] = min_d[2*k+1];
    } else if(min_v[2*k+1] > min_v[2*k+2]) {
      min_v[k] = min_v[2*k+2];
      min_c[k] = min_c[2*k+2];
      smin_v[k] = min(min_v[2*k+1], smin_v[2*k+2]);

      min_d[k] = min_d[2*k+2];
    } else {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1] + min_c[2*k+2];
      smin_v[k] = min(smin_v[2*k+1], smin_v[2*k+2]);

      min_d[k].merge(min_d[2*k+1], min_d[2*k+2]);
    }

    // ノードk において ノード2*k+1 の最大値・最小値にならない情報をマージ
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

    // ノードk において ノード2*k+2 の最大値・最小値にならない情報をマージ
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

  // (内部用) d_i の range maximize query
  void _update_dmax(int k, int l, int r) {
    // a_i が最大値・最小値・それ以外で分けた d_i について d_i の最小値が 0 以上であれば終了
    if(l == r || (0 <= max_d[k].min_v && 0 <= nval_d[k].min_v && 0 <= min_d[k].min_v)) {
      return;
    }
    // a_i が最大値・最小値・それ以外で分けた d_i について
    // d_i の2番目の最小値が 0 より大きければ d_i の最大値を更新
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

  // a_i の range minimize query
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

  // a_i の range maximize query
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

  // a_i の range add query
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

  // b_i の range maximum query
  ll _query_hmax_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      // a_i が最大値・最小値・それ以外のうちの a_i + d_i の最大値を返す
      return max(max(max_d[k].hmax(), min_d[k].hmax()), nval_d[k].hmax());
    }
    push(k);
    ll lv = _query_hmax_max(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmax_max(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

  // b_i の range sum query
  ll _query_hmax_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      if(max_v[k] == min_v[k]) {
        // 同じ要素が a_i が最大値の要素、a_i が最小値の要素両方に含まれている
        // 片方の総和のみ足して返す
        return sum[k] + max_d[k].min_s;
      }
      // a_i の総和と a_i が最小値、最大値、それ以外の d_i の総和を合わせる
      return sum[k] + max_d[k].min_s + nval_d[k].min_s + min_d[k].min_s;
    }
    push(k);
    ll lv = _query_hmax_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmax_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

public:
  SegmentTree(int n, ll *a) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<2*n0-1; ++i) ladd[i] = 0;
    len[0] = n0;
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = min_v[n0-1+i] = sum[n0-1+i] = a[i];
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

  // i<=i<r について A_i の値を max(A_i, x) に更新
  ll update_max(int a, int b, ll x) {
    _update_max(x, a, b, 0, 0, n0);
  }

  // i<=i<r について A_i の値を min(A_i, x) に更新
  ll update_min(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
  }

  // l<=i<r について A_i の値に x を加える
  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  // l<=i<r について A_i の値を x に更新
  void update_val(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
    _update_max(x, a, b, 0, 0, n0);
  }

  // l<=i<r の中の B_i の最大値を求める
  ll query_hmax_max(int a, int b) {
    return _query_hmax_max(a, b, 0, 0, n0);
  }

  // l<=i<r の中の B_i の総和を求める
  ll query_hmax_sum(int a, int b) {
    return _query_hmax_sum(a, b, 0, 0, n0);
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

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = 1000; //szrnd(mt);
  uniform_int_distribution<int> rtype(0, 5), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  const int limit = 2000;

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
          }
          break;
        case 1:
          printf("%d: update_val (%d, %d) : %lld\n", c, a, b, x);
          stb.update_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = x;
            w[i] = max(v[i], w[i]);
          }
          break;
        case 2:
          printf("%d: query_hmax_max (%d, %d)\n", c, a, b);
          r0 = stb.query_hmax_max(a, b);
          r1 = -1e18;
          for(int i=a; i<b; ++i) {
            r1 = max(r1, w[i]);
          }
          if(r0 != r1) {
            printf("query max (%d, %d) : result is %lld, expected is %lld\n", a, b, r0, r1);
            wrong = true;
          }
          break;
        case 3:
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
        case 4:
          printf("%d: update_min (%d, %d): %lld\n", c, a, b, x);
          stb.update_min(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = min(v[i], x);
          }
          break;
        case 5:
          printf("%d: update_max (%d, %d): %lld\n", c, a, b, x);
          stb.update_max(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = max(v[i], x);
            w[i] = max(w[i], v[i]);
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
