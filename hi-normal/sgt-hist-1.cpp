#include<algorithm>
#include<random>
#include<iostream>
#include<sstream>
using namespace std;
using ll = long long;

// Segment Tree (with historic information)
// - l<=i<r について、 A_i の値を x に更新
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の中の A_i の最小値を求める
// - l<=i<r の A_i の和を求める
// - l<=i<r の中の B_i の最大値を求める
// - l<=i<r の中の C_i の最小値を求める
// - (各クエリ後、B_i は max(A_i, B_i) に更新)
// - (各クエリ後、C_i は min(A_i, C_i) に更新)

#define N 10003

class SegmentTree {
  const ll inf = 1e18;

  struct Val {
    ll vl, ma, mi;
    Val() {}
    Val(ll v) : vl(v), ma(v), mi(v) {}
    void init(ll vl, ll ma, ll mi) { this->vl = vl; this->ma = ma; this->mi = mi; }
    void set(ll v) { vl = ma = mi = v; }
    void add(ll v) {
      vl += v; ma = max(vl, ma); mi = min(vl, mi);
    }
    void add(Val &v) {
      ma = max(ma, vl + v.ma);
      mi = min(mi, vl + v.mi);
      vl += v.vl;
    }
    void update(Val &x, Val &a) {
      ma = max(ma, max(vl + a.ma, x.ma));
      mi = min(mi, min(vl + a.mi, x.mi));
      vl = x.vl;
    }

    string print() {
      stringstream ss;
      ss << "{" << vl << ", " << ma << ", " << mi << "}";
      return ss.str();
    }
  };
  int n, n0;
  Val max_v[4*N], min_v[4*N];
  ll sum[4*N], num[4*N];
  Val lval[4*N], ladd[4*N];

  void push(int k) {
    if(n0-1 <= k) return;

    if(lval[k].vl != inf) {
      updateall(2*k+1, lval[k], ladd[k]);
      updateall(2*k+2, lval[k], ladd[k]);
      lval[k].set(inf);
      ladd[k].set(0);
      return;
    }

    if(ladd[k].vl != 0) {
      addall(2*k+1, ladd[k]);
      addall(2*k+2, ladd[k]);
      ladd[k].set(0);
    }
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    max_v[k].vl = max(max_v[2*k+1].vl, max_v[2*k+2].vl);
    max_v[k].ma = max(max_v[2*k+1].ma, max_v[2*k+2].ma);

    min_v[k].vl = min(min_v[2*k+1].vl, min_v[2*k+2].vl);
    min_v[k].mi = min(min_v[2*k+1].mi, min_v[2*k+2].mi);
  }

  ll _query_max(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return max_v[k].vl;
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
      return min_v[k].vl;
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

  // a: addする値の lazy value
  void addall(int k, Val a) {
    max_v[k].add(a);
    min_v[k].add(a);

    sum[k] += num[k] * a.vl;
    if(lval[k].vl != inf) {
      lval[k].add(a);
    } else {
      ladd[k].add(a);
    }
  }

  // x: updateする値の lazy value
  // a: addする値の lazy value
  // これらの値は、"add した後に update される" 関係にある
  void updateall(int k, Val x, Val a) {
    max_v[k].update(x, a);
    min_v[k].update(x, a);

    sum[k] = x.vl * num[k];
    if(lval[k].vl == inf) {
      ladd[k].add(a);
      lval[k].ma = x.ma;
      lval[k].mi = x.mi;
      lval[k].vl = x.vl;
    } else {
      lval[k].update(x, a);
    }
  }

  void _add(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, Val(x));
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
      updateall(k, Val(x), Val(0));
      return;
    }

    push(k);
    _update_val(x, a, b, 2*k+1, l, (l+r)/2);
    _update_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_hmax(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return max_v[k].ma;
    }
    push(k);
    ll lv = _query_hmax(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmax(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

  ll _query_hmin(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return inf;
    }
    if(a <= l && r <= b) {
      return min_v[k].mi;
    }
    push(k);
    ll lv = _query_hmin(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmin(a, b, 2*k+2, (l+r)/2, r);
    return min(lv, rv);
  }

  public:
  SegmentTree(int n) {
    SegmentTree(n, nullptr);
  }

  SegmentTree(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    if(a != nullptr) {
      for(int i=0; i<n; ++i) {
        sum[n0-1+i] = a[i];
        max_v[n0-1+i].set(a[i]);
        min_v[n0-1+i].set(a[i]);

        lval[n0-1+i].set(inf);
        ladd[n0-1+i].set(0);
        num[n0-1+i] = 1;
      }
    } else {
      for(int i=0; i<n; ++i) {
        sum[n0-1+i] = 0;
        max_v[n0-1+i].set(0);
        min_v[n0-1+i].set(0);

        lval[n0-1+i].set(inf);
        ladd[n0-1+i].set(0);
        num[n0-1+i] = 1;
      }
    }
    for(int i=n; i<n0; ++i) {
      max_v[n0-1+i].set(-inf);
      min_v[n0-1+i].set(inf);
      sum[n0-1+i] = 0;

      lval[n0-1+i].set(inf);
      ladd[n0-1+i].set(0);
      num[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; i--) {
      num[i] = num[2*i+1] + num[2*i+2];
      lval[i].set(inf);
      ladd[i].set(0);
      update(i);
    }
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

  void add(int a, int b, ll x) {
    _add(x, a, b, 0, 0, n0);
  }

  void update_val(int a, int b, ll x) {
    _update_val(x, a, b, 0, 0, n0);
  }

  ll query_hmax(int a, int b) {
    return _query_hmax(a, b, 0, 0, n0);
  }

  ll query_hmin(int a, int b) {
    return _query_hmin(a, b, 0, 0, n0);
  }

  void debug() {
    for(int i=0; i<2*n0; ++i) {
      printf("* %d: mx = %s, mi = %s, sm = %lld, lvl = %s, lad = %s\n",
          i, max_v[i].print().c_str(), min_v[i].print().c_str(), sum[i],
          lval[i].print().c_str(), ladd[i].print().c_str()
          );
    }
  }
};

ll v[N], w[N], u[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  uniform_int_distribution<ll> val(0, 1e10);
  //uniform_int_distribution<ll> val(0, 100);

  bool ipt = false;
  int cs = 0;
  if(ipt) cin >> cs;

  while(!ipt || cs--) {
    int n = 128; //szrnd(mt);
    stringstream ss;
    if(ipt) {
      cin >> n;
      cout << n << endl;
    }
    ss << n << "\n";
    uniform_int_distribution<int> rtype(0, 6), gen(0, n);

    for(int i=0; i<n; ++i) v[i] = u[i] = w[i] = val(mt);
    if(ipt) {
      for(int i=0; i<n; ++i) {
        cin >> v[i]; w[i] = u[i] = v[i];
      }
    }
    for(int i=0; i<n; ++i) {
      if(i) ss << " "; ss << v[i];
    }
    ss << "\n";
    printf("=== start\n");
    for(int i=0; i<n; ++i) printf("%lld ", v[i]); printf("\n");
    SegmentTree stb(n, v);
    int t, a, b;
    int c = 1, d = 200000;
    if(ipt) {
      cin >> d;
    }
    bool wrong = false;
    ll x, r0, r1;
    while((ipt || !wrong) && c <= d) {
      t = rtype(mt);
      a = gen(mt); b = gen(mt);
      x = val(mt);
      if(ipt) {
        cin >> t >> a >> b >> x;
      }
      if(a == b) continue;
      if(a > b) swap(a, b);
      switch(t) {
        case 0:
          printf("%d: query_max(%d, %d)\n", c, a, b);
          r0 = stb.query_max(a, b);
          r1 = -1e18;
          for(int i=a; i<b; ++i) {
            if(r1 < v[i]) r1 = v[i];
          }
          if(r0 != r1) {
            cout << "(wrong) query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
            wrong = true;
          }
          break;
        case 1:
          printf("%d: query_min(%d, %d)\n", c, a, b);
          r0 = stb.query_min(a, b);
          r1 = 1e18;
          for(int i=a; i<b; ++i) {
            if(v[i] < r1) r1 = v[i];
          }
          if(r0 != r1) {
            cout << "(wrong) query min (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
            wrong = true;
          }
          break;
        case 2:
          printf("%d: query_sum(%d, %d)\n", c, a, b);
          r0 = stb.query_sum(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 += v[i];
          }
          if(r0 != r1) {
            cout << "(wrong) query sum (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
            wrong = true;
          }
          break;
        case 3:
          printf("%d: add(%d, %d, %lld)\n", c, a, b, x);
          stb.add(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] += x;
            w[i] = max(v[i], w[i]);
            u[i] = min(v[i], u[i]);
          }
          break;
        case 4:
          printf("%d: update_val(%d, %d, %lld)\n", c, a, b, x);
          stb.update_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = x;
            w[i] = max(v[i], w[i]);
            u[i] = min(v[i], u[i]);
          }
          break;
        case 5:
          printf("%d: query_hmax(%d, %d)\n", c, a, b);
          r0 = stb.query_hmax(a, b);
          r1 = -1e18;
          for(int i=a; i<b; ++i) {
            r1 = max(r1, w[i]);
          }
          if(r0 != r1) {
            cout << "(wrong) query hmax (" << a << ", " << b << ") : " << r0 << (r0 < r1 ? " < " : " > ") << r1 << endl;
            wrong = true;
          }
          break;
        case 6:
          printf("%d: query_hmin(%d, %d)\n", c, a, b);
          r0 = stb.query_hmin(a, b);
          r1 = 1e18;
          for(int i=a; i<b; ++i) {
            r1 = min(r1, u[i]);
          }
          if(r0 != r1) {
            cout << "(wrong) query hmin (" << a << ", " << b << ") : " << r0 << (r0 < r1 ? " < " : " > ") << r1 << endl;
            wrong = true;
          }
          break;
        default:
          continue;
      }
      ss << t << " " << a << " " << b << " " << x << "\n";
      ++c;
      //stb.print();
      //stb.debug();
    }
    if(!ipt && wrong) {
      cout << ss.str() << endl;
      break;
    }
  }
  return 0;
}
