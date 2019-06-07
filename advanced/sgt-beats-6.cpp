#include<algorithm>
#include<random>
#include<iostream>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について A_i の値を min(A_i, x) に更新
// - l<=i<r について A_i の値を A_i + x に更新
// - l < r について gcd(A_l, A_{l+1}, ..., A_{r-1}) を求める
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の A_i の和を求める

#define N 10003

class SegmentTree {
  const ll inf = 1e18;
  int n, n0;
  ll max_v[4*N], smax_v[4*N];
  ll sum[4*N], max_c[4*N];

  ll len[4*N];
  ll ladd[4*N];
  ll s[4*N], num[4*N];

  ll gcd(ll m, ll n) {
    if(m == 0) return n;
    if(n == 0) return m;

    ll r = m % n;
    return r ? gcd(n, r) : n;
  }

  void update_node_max(int k, ll x) {
    sum[k] += (x - max_v[k]) * max_c[k];
    max_v[k] = x;
  }

  void addall(int k, ll x) {
    max_v[k] += x;
    if(smax_v[k] != -inf) {
      smax_v[k] += x;
      s[k] += x;
    }

    sum[k] += x * len[k];
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
    sum[k] = sum[2*k+1] + sum[2*k+2];
    num[k] = gcd(num[2*k+1], num[2*k+2]);

    if(max_v[2*k+1] < max_v[2*k+2]) {
      max_v[k] = max_v[2*k+2];
      max_c[k] = max_c[2*k+2];
      smax_v[k] = max(max_v[2*k+1], smax_v[2*k+2]);

      s[k] = max_v[2*k+1];
    } else if(max_v[2*k+1] > max_v[2*k+2]) {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1];
      smax_v[k] = max(smax_v[2*k+1], max_v[2*k+2]);

      s[k] = max_v[2*k+2];
    } else {
      max_v[k] = max_v[2*k+1];
      max_c[k] = max_c[2*k+1] + max_c[2*k+2];
      smax_v[k] = max(smax_v[2*k+1], smax_v[2*k+2]);

      s[k] = max(s[2*k+1], s[2*k+2]);
    }
    if(s[2*k+1]) num[k] = gcd(num[k], abs(s[k] - s[2*k+1]));
    if(s[2*k+2]) num[k] = gcd(num[k], abs(s[k] - s[2*k+2]));
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
      return sum[k];
    }
    push(k);
    ll lv = _query_sum(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_sum(a, b, 2*k+2, (l+r)/2, r);
    return lv + rv;
  }

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      return;
    }
    push(k);
    _add_val(x, a, b, 2*k+1, l, (l+r)/2);
    _add_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_gcd(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return 0;
    }
    if(a <= l && r <= b) {
      return gcd(gcd(max_v[k], max_v[k] - s[k]), num[k]);
    }
    push(k);
    ll lv = _query_gcd(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_gcd(a, b, 2*k+2, (l+r)/2, r);
    return gcd(lv, rv);
  }

public:
  SegmentTree(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<2*n0-1; ++i) ladd[i] = 0;
    len[0] = n0;
    for(int i=0; i<n0-1; ++i) len[2*i+1] = len[2*i+2] = (len[i] >> 1);

    for(int i=0; i<n; ++i) {
      max_v[n0-1+i] = sum[n0-1+i] = a[i];
      smax_v[n0-1+i] = -inf;
      max_c[n0-1+i] = 1;

      num[n0-1+i] = 0;
    }

    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i] = smax_v[n0-1+i] = -inf;
      sum[n0-1+i] = max_c[n0-1+i] = 0;

      num[n0-1+i] = 0;
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

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  ll query_gcd(int a, int b) {
    return _query_gcd(a, b, 0, 0, n0);
  }

  void debug() {
    for(int k=0; k<2*n0-1; ++k) {
      printf("%d: max = {%lld, %lld, %lld}, sum = %lld, s = %lld, num = %lld, ladd = %lld\n",
          k, max_v[k], smax_v[k], max_c[k], sum[k], s[k], num[k], ladd[k]
          );
    }
  }
};

ll v[N];

ll gcd(ll m, ll n) {
  if(m == 0) return n;
  if(n == 0) return m;
  ll r = m % n;
  return r ? gcd(n, r) : n;
}

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  int n = szrnd(mt);
  uniform_int_distribution<int> rtype(0, 4), gen(0, n);
  const int limit = 100000, alimit = 10000;
  uniform_int_distribution<ll> val(1, 1e10), val2(-alimit, alimit), val3(1, alimit);


  while(1) {
    cout << "=================" << endl;
    for(int i=0; i<n; ++i) v[i] = val(mt);
    SegmentTree stb(n, v);
    int a, b;
    bool wrong = false;
    //stb.debug();
    int c = 1;
    ll x, r0, r1;
    while(c <= limit && !wrong) {
      a = gen(mt); b = gen(mt);
      if(a == b) continue;
      if(a > b) swap(a, b);
      x = val(mt);
      switch(rtype(mt)) {
        case 0:
          printf("%d: update min (%d, %d): %lld\n", c, a, b, x);
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
            wrong = true;
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
            wrong = true;
            cout << "query sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
          }
          break;
        case 3:
          r0 = 1e18;
          for(int i=a; i<b; ++i) {
            r0 = min(r0, v[i]);
          }
          x = r0 > alimit ? val2(mt) : val3(mt);
          printf("%d: add val (%d, %d): %lld\n", c, a, b, x);
          stb.add_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] += x;
          }
          break;
        case 4:
          r0 = stb.query_gcd(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 = gcd(v[i], r1);
          }
          printf("%d: query gcd (%d, %d) : %lld %lld\n", c, a, b, r0, r1);
          if(r0 != r1) {
            wrong = true;
            cout << ">>> "; for(int i=0; i<n; ++i) cout << v[i] << " "; cout << endl;
          }
          break;
        default:
          continue;
      }
      //stb.debug();
      ++c;
    }
    if(wrong) break;
  }
  return 0;
}
