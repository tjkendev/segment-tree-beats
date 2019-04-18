#include<algorithm>
#include<random>
#include<iostream>
#include<cassert>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について、 A_i の値を min(A_i, x) に更新
// - l<=i<r について、 A_i の値を max(A_i, x) に更新
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の A_i の最小値を求める
// - l<=i<r の A_i の和を求める
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r について、 A_i の値を x に更新

#define N 10003

class SegmentTreeBeats {
  const ll inf = 1e18;
  int n, n0;
  ll max_v[4*N], smax_v[4*N], max_c[4*N];
  ll min_v[4*N], smin_v[4*N], min_c[4*N];
  ll sum[4*N];
  ll ecnt[4*N], ladd[4*N], lval[4*N];

  void update_node_max(int k, ll x) {
    sum[k] += (x - max_v[k]) * max_c[k];

    if(max_v[k] == min_v[k]) {
      max_v[k] = min_v[k] = x;
    } else if(max_v[k] == smin_v[k]) {
      max_v[k] = smin_v[k] = x;
    } else {
      max_v[k] = x;
    }

    if(lval[k] != inf && x < lval[k]) {
      lval[k] = x;
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

    if(lval[k] != inf && lval[k] < x) {
      lval[k] = x;
    }
  }

  void push(int k) {

    if(n0-1 <= k) return;

    if(lval[k] != inf) {
      updateall(2*k+1, lval[k]);
      updateall(2*k+2, lval[k]);
      lval[k] = inf;
      return;
    }

    if(ladd[k] != 0) {
      addall(2*k+1, ladd[k]);
      addall(2*k+2, ladd[k]);
      ladd[k] = 0;
    }

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
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

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
      update_node_min(k, x);
      return;
    }

    push(k);
    _update_max(x, a, b, 2*k+1, l, (l+r)/2);
    _update_max(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void addall(int k, ll x) {
    max_v[k] += x;
    if(smax_v[k] != -inf) smax_v[k] += x;
    min_v[k] += x;
    if(smin_v[k] != inf) smin_v[k] += x;

    sum[k] += ecnt[k] * x;
    if(lval[k] != inf) {
      lval[k] += x;
    } else {
      ladd[k] += x;
    }
  }

  void updateall(int k, ll x) {
    max_v[k] = x; smax_v[k] = -inf;
    min_v[k] = x; smin_v[k] = inf;
    max_c[k] = min_c[k] = ecnt[k];

    sum[k] = x * ecnt[k];
    lval[k] = x; ladd[k] = 0;
  }

  void _add(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, x);
      return;
    }

    push(k);
    _add(x, a, b, 2*k+1, l, (l+r)/2);
    _add(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      updateall(k, x);
      return;
    }

    push(k);
    _update_val(x, a, b, 2*k+1, l, (l+r)/2);
    _update_val(x, a, b, 2*k+2, (l+r)/2, r);
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

public:
  SegmentTreeBeats(int n) {
    SegmentTreeBeats(n, nullptr);
  }

  SegmentTreeBeats(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    if(a != nullptr) {
      for(int i=0; i<n; ++i) {
        max_v[n0-1+i] = min_v[n0-1+i] = sum[n0-1+i] = a[i];
        smax_v[n0-1+i] = -inf;
        smin_v[n0-1+i] = inf;
        max_c[n0-1+i] = min_c[n0-1+i] = 1;

        ecnt[n0-1+i] = 1; ladd[n0-1+i] = 0;
        lval[n0-1+i] = inf;
      }
    } else {
      for(int i=n; i<n0; ++i) {
        max_v[n0-1+i] = min_v[n0-1+i] = sum[n0-1+i] = 0;
        smax_v[n0-1+i] = -inf;
        smin_v[n0-1+i] = inf;
        max_c[n0-1+i] = min_c[n0-1+i] = 1;

        ecnt[n0-1+i] = 1; ladd[n0-1+i] = 0;
        lval[n0-1+i] = inf;
      }
    }
    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i] = smax_v[n0-1+i] = 0;
      min_v[n0-1+i] = smin_v[n0-1+i] = 0;
      max_c[n0-1+i] = min_c[n0-1+i] = 0;

      ecnt[n0-1+i] = ladd[n0-1+i] = 0;
      lval[n0-1+i] = inf;
    }
    for(int i=n0-2; i>=0; i--) {
      update(i);
      ecnt[i] = ecnt[2*i+1] + ecnt[2*i+2];
      ladd[i] = 0; lval[i] = inf;
    }
  }

  void update_min(int a, int b, ll x) {
    _update_min(x, a, b, 0, 0, n0);
  }

  void update_max(int a, int b, ll x) {
    _update_max(x, a, b, 0, 0, n0);
  }

  void add(int a, int b, ll x) {
    _add(x, a, b, 0, 0, n0);
  }

  void update_val(int a, int b, ll x) {
    _update_val(x, a, b, 0, 0, n0);
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

  vector<ll> dv;
  void _print(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) return;
    if(r-l == 1) {
      dv.push_back(max_v[k]);
      return;
    }
    push(k);
    _print(a, b, 2*k+1, l, (l+r)/2);
    _print(a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }
  void print() {
    dv.clear();
    _print(0, n, 0, 0, n0);
    for(ll v : dv) cout << " " << v; cout << endl;
  }

  void debug() {
    for(int i=0; i<2*n0; ++i) {
      printf("%d: mx = %lld, smx = %lld, mc = %lld, mn = %lld, smn = %lld, mc = %lld "
          "sm = %lld, lad = %lld, lvl = %lld\n",
          i, max_v[i], smax_v[i], max_c[i], min_v[i], smin_v[i], min_c[i], sum[i], ladd[i], lval[i]);
    }
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(1000, 10000);
  uniform_int_distribution<ll> val(0, 1e10);

  int n = szrnd(mt);

  //cin >> n;
  uniform_int_distribution<int> rtype(0, 6), gen(0, n);


  while(1) {
    //cout << "=== begin === " << endl;
  //for(int i=0; i<n; ++i) cin >> v[i];
  for(int i=0; i<n; ++i) v[i] = val(mt);
  SegmentTreeBeats stb(n, v);
  int a, b, t;
  ll x, r0, r1;
  int c = 0;
  bool show = false;
  bool wrong = false;

  //for(int i=0; i<n; ++i) cout << " " << v[i]; cout << endl;
  while(++c && !wrong) {
    t = rtype(mt);
    a = gen(mt); b = gen(mt);
    x = val(mt);
    //cin >> t >> a >> b >> x;
    if(show) {
      cout << c << " : ";
    }
    if(a == b) continue;
    if(a > b) swap(a, b);
    switch(t) {
      case 0:
        if(show) {
          cout << "update min (" << a << ", " << b << ") = " << x << endl;
        }
        stb.update_min(a, b, x);
        for(int i=a; i<b; ++i) {
          if(x < v[i]) v[i] = x;
        }
        break;
      case 1:
        if(show) {
          cout << "update max (" << a << ", " << b << ") = " << x << endl;
        }
        stb.update_max(a, b, x);
        for(int i=a; i<b; ++i) {
          if(v[i] < x) v[i] = x;
        }
        break;
      case 2:
        r0 = stb.query_max(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          if(r1 < v[i]) r1 = v[i];
        }
        if(show || r0 != r1) {
          wrong |= (r0 != r1);
          cout << "query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 3:
        r0 = stb.query_min(a, b);
        r1 = (1e18);
        for(int i=a; i<b; ++i) {
          if(v[i] < r1) r1 = v[i];
        }
        if(show || r0 != r1) {
          wrong |= (r0 != r1);
          cout << "query min (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 4:
        r0 = stb.query_sum(a, b);
        r1 = 0;
        for(int i=a; i<b; ++i) {
          r1 += v[i];
        }
        if(show || r0 != r1) {
          wrong |= (r0 != r1);
          cout << "query sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
        }
        break;
      case 5:
        stb.add(a, b, x);
        for(int i=a; i<b; ++i) {
          v[i] += x;
        }
        if(show) {
          cout << "add " << x << " to (" << a << ", " << b << ")" << endl;
        }
        break;
      case 6:
        stb.update_val(a, b, x);
        for(int i=a; i<b; ++i) {
          v[i] = x;
        }
        if(show) {
          cout << "update to " << x << " in (" << a << ", " << b << ")" << endl;
        }
        break;
      default:
        continue;
    }
    if(false && show) {
      cout << "expected:";
      for(int i=0; i<n; ++i) cout << " " << v[i]; cout << endl;
      cout << "  actual:";
      stb.print();
    }
    //stb.debug();
  }
  if(wrong) break;
  }
  return 0;
}
