#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <bits/stdc++.h>

template<class T> void debug_out() { std::cerr << std::endl; }
template<class T> std::ostream& prnt(std::ostream& out, T v) { out << v.size() << '\n'; for (auto e : v) out << e << ' '; return out; }
template<class T> std::ostream& operator<<(std::ostream& out, std::vector<T> v) { return prnt(out, v); }
template<class T> std::ostream& operator<<(std::ostream& out, std::set<T> v) { return prnt(out, v); }
template<class T> std::ostream& operator<<(std::ostream& out, std::unordered_set<T> v) { return prnt(out, v); }
template<class T1, class T2> std::ostream& operator<<(std::ostream& out, std::map<T1, T2> v) { return prnt(out, v); }
template<class T1, class T2> std::ostream& operator<<(std::ostream& out, std::pair<T1, T2> p) { return out << '(' << p.first << ' ' << p.second << ')'; }
template<class Head, class... Tail> void debug_out(Head H, Tail... T) { std::cerr << ' ' << H; debug_out<Head>(T...); }

#define dbg(...) std::cerr << #__VA_ARGS__ << " ->", debug_out(__VA_ARGS__)
#define dbg_v(x, n) do { std::cerr << #x"[]: "; for (int _ = 0; _ < n; ++_) std::cerr << x[_] << ' '; std::cerr << '\n'; } while (false)
#define dbg_ok std::cerr << "OK!\n"

#endif
