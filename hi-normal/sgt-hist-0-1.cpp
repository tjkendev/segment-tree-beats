#include<algorithm>
#include<random>
#include<iostream>
#include<sstream>
using namespace std;
using ll = long long;

// Segment Tree with Lazy Propagation (historic information)
// - l<=i<r について、 a_i の値を x に更新
// - l<=i<r について、 a_i の値に x を加える
// - l<=i<r の中の a_i の最大値を求める
// - l<=i<r の中の b_i の最大値を求める
// - (各クエリ後、b_i は max(a_i, b_i) に更新)

#define N 10003

class SegmentTree {
  static const ll inf = 1e16;

  struct Tag {
    ll ha, hb, a, b;

    Tag() {}

    Tag(ll a, ll b) : a(a), b(b) { ha = a; hb = b; }

    ll calc(ll x) const { return max(x + a, b); }
    ll hcalc(ll x) const { return max(x + ha, hb); }

    void merge(Tag &t) {
      ha = max(ha, a + t.ha);
      hb = max(max(hb, b + t.ha), t.hb);

      a = a + t.a;
      b = max(b + t.a, t.b);
    }

    void clear() { a = ha = 0; b = hb = -inf; }

    bool empty() const { return a == 0 && b == -inf; }
  };

  int n0;
  ll max_v[4*N], hmax_v[4*N];
  Tag lazy[4*N];

  void push(int k) {
    if(n0-1 <= k) return;

    if(!lazy[k].empty()) {
      hmax_v[2*k+1] = max(hmax_v[2*k+1], lazy[k].hcalc(max_v[2*k+1]));
      max_v[2*k+1] = lazy[k].calc(max_v[2*k+1]);
      lazy[2*k+1].merge(lazy[k]);

      hmax_v[2*k+2] = max(hmax_v[2*k+2], lazy[k].hcalc(max_v[2*k+2]));
      max_v[2*k+2] = lazy[k].calc(max_v[2*k+2]);
      lazy[2*k+2].merge(lazy[k]);

      lazy[k].clear();
    }
  }

  void update(int k) {
    max_v[k] = max(max_v[2*k+1], max_v[2*k+2]);
    hmax_v[k] = max(hmax_v[2*k+1], hmax_v[2*k+2]);
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

  void _add_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      Tag t = Tag(x, -inf);
      hmax_v[k] = max(hmax_v[k], t.hcalc(max_v[k]));
      max_v[k] = t.calc(max_v[k]);
      lazy[k].merge(t);
      return;
    }

    push(k);
    _add_val(x, a, b, 2*k+1, l, (l+r)/2);
    _add_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_val(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      Tag t = Tag(-inf, x);
      hmax_v[k] = max(hmax_v[k], t.hcalc(max_v[k]));
      max_v[k] = t.calc(max_v[k]);
      lazy[k].merge(t);
      return;
    }

    push(k);
    _update_val(x, a, b, 2*k+1, l, (l+r)/2);
    _update_val(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  void _update_max(ll x, int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return;
    }
    if(a <= l && r <= b) {
      Tag t = Tag(0, x);
      hmax_v[k] = max(hmax_v[k], t.hcalc(max_v[k]));
      max_v[k] = t.calc(max_v[k]);
      lazy[k].merge(t);
      return;
    }

    push(k);
    _update_max(x, a, b, 2*k+1, l, (l+r)/2);
    _update_max(x, a, b, 2*k+2, (l+r)/2, r);
    update(k);
  }

  ll _query_hmax(int a, int b, int k, int l, int r) {
    if(b <= l || r <= a) {
      return -inf;
    }
    if(a <= l && r <= b) {
      return hmax_v[k];
    }
    push(k);
    ll lv = _query_hmax(a, b, 2*k+1, l, (l+r)/2);
    ll rv = _query_hmax(a, b, 2*k+2, (l+r)/2, r);
    return max(lv, rv);
  }

public:
  SegmentTree(int n, ll *a) {
    n0 = 1;
    while(n0 < n) n0 <<= 1;

    for(int i=0; i<2*n0-1; ++i) lazy[i].clear();

    for(int i=0; i<n; ++i) max_v[n0-1+i] = hmax_v[n0-1+i] = a[i];
    for(int i=n; i<n0; ++i) max_v[n0-1+i] = hmax_v[n0-1+i] = -inf;

    for(int i=n0-2; i>=0; --i) update(i);
  }

  void add_val(int a, int b, ll x) {
    _add_val(x, a, b, 0, 0, n0);
  }

  void update_val(int a, int b, ll x) {
    _update_val(x, a, b, 0, 0, n0);
  }

  void update_max(int a, int b, ll x) {
    _update_max(x, a, b, 0, 0, n0);
  }

  ll query_max(int a, int b) {
    return _query_max(a, b, 0, 0, n0);
  }

  ll query_hmax(int a, int b) {
    return _query_hmax(a, b, 0, 0, n0);
  }
};

ll v[N], w[N];

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
    uniform_int_distribution<int> rtype(1, 5), gen(0, n);

    for(int i=0; i<n; ++i) v[i] = w[i] = val(mt);
    if(ipt) {
      for(int i=0; i<n; ++i) {
        cin >> v[i]; w[i] = v[i];
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
        case 1:
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
        case 2:
          printf("%d: add(%d, %d, %lld)\n", c, a, b, x);
          stb.add_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] += x;
            w[i] = max(v[i], w[i]);
          }
          break;
        case 3:
          printf("%d: update_val(%d, %d, %lld)\n", c, a, b, x);
          stb.update_val(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = x;
            w[i] = max(v[i], w[i]);
          }
          break;
        case 4:
          printf("%d: update_max(%d, %d, %lld)\n", c, a, b, x);
          stb.update_max(a, b, x);
          for(int i=a; i<b; ++i) {
            v[i] = max(v[i], x);
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
      ss << t << " " << a << " " << b << " " << x << "\n";
      ++c;
      //stb.print();
      //stb.debug();
    }
    if(!ipt && wrong) {
      //cout << ss.str() << endl;
      break;
    }
  }
  return 0;
}
