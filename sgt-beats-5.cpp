#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について、 A_i の値を min(A_i, x) に更新
// - l<=i<r について、 A_i の値を max(A_i, x) に更新
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の A_i の最小値を求める
// - l<=i<r の A_i の和を求める
// - l<=i<r の B_i の和を求める
//
// 各クエリで、A_i が変化した場合に B_i に1加算する

#define N 10003

class SegmentTreeBeats {
  const ll inf = 1e18;
  int n, n0;
  ll max_v[4*N], smax_v[4*N], max_c[4*N];
  ll min_v[4*N], smin_v[4*N], min_c[4*N];
  ll sum[4*N];
  ll sum_b[4*N], min_a[4*N], max_a[4*N];

  void update_node_max(int k, ll x) {
    sum[k] += (x - max_v[k]) * max_c[k];

    if(max_v[k] == min_v[k]) {
      max_v[k] = min_v[k] = x;
    } else if(max_v[k] == smin_v[k]) {
      max_v[k] = smin_v[k] = x;
    } else {
      max_v[k] = x;
    }
  }
  void update_node_min(int k, ll x) {
    sum[k] += (x - min_v[k]) * min_c[k];

    if(max_v[k] == min_v[k]) {
      max_v[k] = min_v[k] = x;
    } else if(smax_v[k] == min_v[k]) {
      min_v[k] = smax_v[k] = x;
    } else {
      min_v[k] = x;
    }
  }

  void add_b(int p, int k, ll mi, ll ma) {
    if(p == -1 || min_v[p] == min_v[k]) {
      sum_b[k] += mi * min_c[k];
      min_a[k] += mi;
    }

    if(p == -1 || max_v[p] == max_v[k]) {
      sum_b[k] += ma * max_c[k];
      max_a[k] += ma;
    }
  }

  void push(int k) {
    if(max_v[k] < max_v[2*k+1]) {
      update_node_max(2*k+1, max_v[k]);
    }
    if(min_v[2*k+1] < min_v[k]) {
      update_node_min(2*k+1, min_v[k]);
    }

    if(max_v[k] < max_v[2*k+2]) {
      update_node_max(2*k+2, max_v[k]);
    }
    if(min_v[2*k+2] < min_v[k]) {
      update_node_min(2*k+2, min_v[k]);
    }

    if(min_a[k] != 0 || max_a[k] != 0) {
      add_b(k, 2*k+1, min_a[k], max_a[k]);
      add_b(k, 2*k+2, min_a[k], max_a[k]);

      min_a[k] = max_a[k] = 0;
    }
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];
    sum_b[k] = sum_b[2*k+1] + sum_b[2*k+2];

    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);
    } else {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);
    }

    if(min_v[2*k+1] < min_v[2*k+2]) {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1];
      smin_v[k] = min(smin_v[2*k+1], min_v[2*k+2]);
    } else if(min_v[2*k+1] > min_v[2*k+2]) {
      min_v[k] = min_v[2*k+2];
      min_c[k] = min_c[2*k+2];
      smin_v[k] = min(min_v[2*k+1], smin_v[2*k+2]);
    } else {
      min_v[k] = min_v[2*k+1];
      min_c[k] = min_c[2*k+1] + min_c[2*k+2];
      smin_v[k] = min(smin_v[2*k+1], smin_v[2*k+2]);
    }
  }

  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_v[k] <= x) {
      return;
    }
    if(a <= l && r <= b && smax_v[k] < x) {
      add_b(-1, k, 0, 1);
      update_node_max(k, x);
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
      add_b(-1, k, 1, 0);
      update_node_min(k, x);
      return;
    }

    push(k);
    _update_max(x, a, b, 2*k+1, l, (l+r)/2);
    _update_max(x, a, b, 2*k+2, (l+r)/2, r);
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

  ll _query_sum(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum[k];
    }
    push(k);
    ll lv = _query_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

  ll _query_sum_b(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return sum_b[k];
    }
    push(k);
    ll lv = _query_sum_b(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum_b(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

public:
  SegmentTreeBeats(int n) {
    SegmentTreeBeats(n, nullptr);
  }

  SegmentTreeBeats(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<2*n0-1; ++i) {
      min_a[i] = max_a[i] = sum_b[i] = 0;
    }

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = min_v[n0-1+i] = sum[n0-1+i] = (a != nullptr ? a[i] : 0);
      smax_v[n0-1+i] = -inf;
      smin_v[n0-1+i] = inf;
      max_c[n0-1+i] = min_c[n0-1+i] = 1;
    }
    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i] = smax_v[n0-1+i] = -inf;
      min_v[n0-1+i] = smin_v[n0-1+i] = inf;
      max_c[n0-1+i] = min_c[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; i--) update(i);
  }

  void update_min(int a, int b, ll x) {
    return _update_min(x, a, b, 0, 0, n0);
  }

  void update_max(int a, int b, ll x) {
    return _update_max(x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  ll query_min(int a, int b) {
    return _query_min(a, b, 0, 0, n0);
  }

  ll query_sum(int a, int b) {
    return _query_sum(a, b, 0, 0, n0);
  }

  ll query_sum_b(int a, int b) {
    return _query_sum_b(a, b, 0, 0, n0);
  }

  // デバッグ用
  void debug() {
    printf("*******************\n");
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: ", k);
      printf("max = {%lld, %lld, %lld}, min = {%lld, %lld, %lld}, sum = %lld, max_a = %lld, min_a = %lld, sum_b = %lld\n",
          max_v[k], smax_v[k], max_c[k],
          min_v[k], smin_v[k], min_c[k], sum[k],
          max_a[k], min_a[k], sum_b[k]
          );
    }
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(1000, 10000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 5), gen(0, n);
  uniform_int_distribution<ll> val(-1e10, 1e10);

  int limit = 20000;

  bool show = true, wrong = false;
  while(1) {
    if(show) {
      printf("=================================\n");
    }
    for(int i=0; i<n; ++i) v[i] = val(mt), w[i] = 0;
    SegmentTreeBeats stb(n, v);
    int a, b;
    ll x, r0, r1;
    int c = 0;
    if(show && n <= 8) {
      stb.debug();
    }
    while(c < limit) {
      a = gen(mt); b = gen(mt);
      if(a == b) continue;
      if(a > b) swap(a, b);
      x = val(mt);
      switch(rtype(mt)) {
        case 0:
          if(show) {
            printf("update chmin (%d, %d): %lld\n", a, b, x);
          }
          stb.update_min(a, b, x);
          for(int i=a; i<b; ++i) {
            if(x < v[i]) {
              v[i] = x;
              ++w[i];
            }
          }
          break;
        case 1:
          if(show) {
            printf("update chmax (%d, %d): %lld\n", a, b, x);
          }
          stb.update_max(a, b, x);
          for(int i=a; i<b; ++i) {
            if(v[i] < x) {
              v[i] = x;
              ++w[i];
            }
          }
          break;
        case 2:
          r0 = stb.query_max(a, b);
          r1 = (-1e18);
          for(int i=a; i<b; ++i) {
            if(r1 < v[i]) r1 = v[i];
          }
          if(show || r0 != r1) {
            printf("query max (%d, %d): %lld %lld\n", a, b, r0, r1);
          }
          break;
        case 3:
          r0 = stb.query_min(a, b);
          r1 = (1e18);
          for(int i=a; i<b; ++i) {
            if(v[i] < r1) r1 = v[i];
          }
          if(show || r0 != r1) {
            printf("query min (%d, %d): %lld %lld\n", a, b, r0, r1);
          }
          break;
        case 4:
          r0 = stb.query_sum(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 += v[i];
          }
          if(show || r0 != r1) {
            printf("query sum (%d, %d): %lld %lld\n", a, b, r0, r1);
            wrong = (r0 != r1);
          }
          break;
        case 5:
          r0 = stb.query_sum_b(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 += w[i];
          }
          if(show || r0 != r1) {
            printf("query sum B (%d, %d): %lld %lld\n", a, b, r0, r1);
            wrong = (r0 != r1);
          }
          break;
        default:
          continue;
      }
      if(show && n <= 8) {
        stb.debug();
      }
      if(wrong) {
        break;
      }
      ++c;
    }
    if(wrong) {
      break;
    }
  }
  return 0;
}
