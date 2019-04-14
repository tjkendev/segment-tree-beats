#include<algorithm>
#include<random>
#include<iostream>
#include<sstream>
using namespace std;
using ll = long long;

// Segment Tree Beats
// - l<=i<r について、 A_i の値を min(A_i, x) に更新
// - l<=i<r について、 A_i の値を x に更新
// - l<=i<r について、 A_i の値に x を加える
// - l<=i<r の中の A_i の最大値を求める
// - l<=i<r の A_i の和を求める
// - l<=i<r の中の B_i の最大値を求める
// - (各クエリ後、B_i は max(A_i, B_i) に更新)

#define N 10003

class SegmentTreeBeats {
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
    void change(ll x) { add(x - vl); }
    void change(Val &v) {
      vl = v.vl; ma = max(ma, v.ma); mi = min(mi, v.mi);
    }

    string print() {
      stringstream ss;
      ss << "{" << vl << ", " << ma << ", " << mi << "}";
      return ss.str();
    }
  };
  int n, n0;
  Val max_v[4*N], smax_v[4*N];
  ll hmax[4*N];
  ll sum[4*N], cnt[4*N];
  ll ecnt[4*N];
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
    }

    if(max_v[k].vl < max_v[2*k+1].vl) {
      sum[2*k+1] += (max_v[k].vl - max_v[2*k+1].vl) * cnt[2*k+1];
      max_v[2*k+1].change(max_v[k].vl);

      if(lval[2*k+1].vl != inf && max_v[k].vl < lval[2*k+1].vl) {
        lval[2*k+1].change(max_v[k].vl);
      }
    }
    if(max_v[k].vl < max_v[2*k+2].vl) {
      sum[2*k+2] += (max_v[k].vl - max_v[2*k+2].vl) * cnt[2*k+2];
      max_v[2*k+2].change(max_v[k].vl);

      if(lval[2*k+2].vl != inf && max_v[k].vl < lval[2*k+2].vl) {
        lval[2*k+2].change(max_v[k].vl);
      }
    }

    if(ladd[k].vl != 0) {
      max_v[2*k+1].ma = max(max_v[2*k+1].ma, max_v[2*k+1].vl);
      max_v[2*k+2].ma = max(max_v[2*k+2].ma, max_v[2*k+2].vl);
    }
    ladd[k].set(0);
  }

  void update(int k) {
    sum[k] = sum[2*k+1] + sum[2*k+2];

    max_v[k].vl = max(max_v[2*k+1].vl, max_v[2*k+2].vl);
    if(max_v[2*k+1].vl < max_v[2*k+2].vl) {
      cnt[k] = cnt[2*k+2];
      smax_v[k].vl = max(max_v[2*k+1].vl, smax_v[2*k+2].vl);
    } else if(max_v[2*k+1].vl > max_v[2*k+2].vl) {
      cnt[k] = cnt[2*k+1];
      smax_v[k].vl = max(smax_v[2*k+1].vl, max_v[2*k+2].vl);
    } else {
      cnt[k] = cnt[2*k+1] + cnt[2*k+2];
      smax_v[k].vl = max(smax_v[2*k+1].vl, smax_v[2*k+2].vl);
    }
    max_v[k].ma = max(max_v[2*k+1].ma, max_v[2*k+2].ma);
  }

  void _update_min(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a || max_v[k].vl <= x) {
      return;
    }
    if(a <= l && r <= b && smax_v[k].vl < x) {
      sum[k] += (x - max_v[k].vl) * cnt[k];
      max_v[k].change(x);

      if(lval[k].vl != inf && x < lval[k].vl) {
        lval[k].change(x);
      }
      return;
    }

    push(k);
    _update_min(x, a, b, 2*k+1, l, (l+r)/2);
    _update_min(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
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

  void addall(int k, Val x) {
    printf("addall(%d, %s), max_v[k] = %s\n", k, x.print().c_str(), max_v[k].print().c_str());
    //max_v[k].ma = max(max_v[k].ma, max_v[k].vl + x.vl);
    max_v[k].vl += x.vl;

    if(smax_v[k].vl != -inf) smax_v[k].vl += x.vl;

    sum[k] += ecnt[k] * x.vl;
    if(lval[k].vl != inf) {
      lval[k].ma = max(lval[k].ma, lval[k].vl + x.vl);
      lval[k].vl += x.vl;
    } else {
      ladd[k].add(x);
    }
  }

  void updateall(int k, Val x, Val a) {
    max_v[k].add(a);
    max_v[k].change(x);

    smax_v[k].vl = -inf;
    cnt[k] = ecnt[k];

    sum[k] = x.vl * ecnt[k];
    if(lval[k].vl == inf) {
      ladd[k].add(a);
      lval[k].ma = x.ma; //max_v[k].ma;
      lval[k].vl = x.vl; //max_v[k].vl;
    } else {
      lval[k].ma = max(lval[k].ma, x.ma); //max_v[k].ma;
      lval[k].vl = x.vl; //max_v[k].vl;
    }
  }

  void _add(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      addall(k, Val(x));
      max_v[k].ma = max(max_v[k].ma, max_v[k].vl);
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

  public:
  SegmentTreeBeats(int n) {
    SegmentTreeBeats(n, nullptr);
  }

  SegmentTreeBeats(int n, ll *a) : n(n) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    if(a != nullptr) {
      for(int i=0; i<n; ++i) {
        sum[n0-1+i] = a[i];
        max_v[n0-1+i].set(a[i]);
        smax_v[n0-1+i].vl = -inf;
        cnt[n0-1+i] = 1;

        lval[n0-1+i].set(inf);
        ladd[n0-1+i].set(0);
        ecnt[n0-1+i] = 1;
      }
    } else {
      for(int i=0; i<n; ++i) {
        sum[n0-1+i] = 0;
        max_v[n0-1+i].set(0);
        smax_v[n0-1+i].vl = -inf;
        cnt[n0-1+i] = 1;

        lval[n0-1+i].set(inf);
        ladd[n0-1+i].set(0);
        ecnt[n0-1+i] = 1;
      }
    }
    for(int i=n; i<n0; ++i) {
      sum[n0-1+i] = 0;
      max_v[n0-1+i].set(-inf);
      smax_v[n0-1+i].vl = -inf;
      cnt[n0-1+i] = 0;

      lval[n0-1+i].set(inf);
      ladd[n0-1+i].set(0);
      ecnt[n0-1+i] = 0;
    }
    for(int i=n0-2; i>=0; i--) {
      ecnt[i] = ecnt[2*i+1] + ecnt[2*i+2];
      lval[i].set(inf);
      ladd[i].set(0);
      update(i);
    }
  }

  vector<ll> dv;
  void _print(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) return;
    if(r-l == 1) {
      dv.push_back(max_v[k].vl);
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

  void update_min(int a, int b, ll x) {
    return _update_min(x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
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

  void debug() {
    for(int i=0; i<2*n0; ++i) {
      printf("* %d: mx = %s, smx = %lld, sm = %lld, ct = %lld, lvl = %s, lad = %s\n",
          i, max_v[i].print().c_str(), smax_v[i].vl, sum[i], cnt[i],
          lval[i].print().c_str(), ladd[i].print().c_str()
          );
    }
  }
};

ll v[N], w[N];

int main() {
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> szrnd(100, 1000);
  //uniform_int_distribution<ll> val(0, 1e10);
  uniform_int_distribution<ll> val(0, 100);

  bool ipt = true;

  while(1) {
    int n = 4; //szrnd(mt);
    if(ipt) {
      cin >> n;
    }
    uniform_int_distribution<int> rtype(0, 5), gen(0, n);

    for(int i=0; i<n; ++i) v[i] = w[i] = val(mt);
    if(ipt) {
      for(int i=0; i<n; ++i) {
        cin >> v[i]; w[i] = v[i];
      }
    }
    printf("=== start\n");
    for(int i=0; i<n; ++i) printf("%lld ", v[i]); printf("\n");
    SegmentTreeBeats stb(n, v);
    int t, a, b;
    int c = 1, d = 3;
    if(ipt) {
      cin >> d;
    }
    bool wrong = false;
    ll x, r0, r1;
    while(!wrong && c <= d) {
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
          printf("%d: update_min(%d, %d, %lld)\n", c, a, b, x);
          stb.update_min(a, b, x);
          for(int i=a; i<b; ++i) {
            if(x < v[i]) v[i] = x;
          }
          break;
        case 10:
          printf("%d: query_max(%d, %d)\n", c, a, b);
          r0 = stb.query_max(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            if(r1 < v[i]) r1 = v[i];
          }
          if(r0 != r1) {
            cout << "(wrong) query max (" << a << ", " << b << ") : " << r0 << " " << r1 << endl;
            wrong = true;
          }
          break;
        case 20:
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
          }
          break;
        case 4:
          printf("%d: update_val(%d, %d, %lld)\n", c, a, b, x);
          stb.update_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = x;
            w[i] = max(v[i], w[i]);
          }
          break;
        case 5:
          printf("%d: query_hmax(%d, %d)\n", c, a, b);
          r0 = stb.query_hmax(a, b);
          r1 = 0;
          for(int i=a; i<b; ++i) {
            r1 = max(r1, w[i]);
          }
          if(r0 != r1) {
            cout << "(wrong) query hmax (" << a << ", " << b << ") : " << r0 << (r0 < r1 ? " < " : " > ") << r1 << endl;
            wrong = true;
          }
          break;
        default:
          continue;
      }
      ++c;
      //stb.print();
      stb.debug();
    }
    if(wrong) break;
  }
  return 0;
}
