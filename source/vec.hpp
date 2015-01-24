#pragma once

#include<type_traits>


/* Template tools */

namespace __vec_tools__
{

template <typename T, int Left>
struct Unroller
{
	
	template <typename S, typename ... Args>
	static void unroll(T *ptr, S arg, Args ... args)
	{
		*ptr = static_cast<T>(arg);
		Unroller<T,Left-1>::unroll(ptr+1,args...);
	}
	
};

template <typename T>
struct Unroller<T,0>
{
	
	static void unroll(T *)
	{
		
	}
	
};

template <typename T, typename S, int N>
void memcopy(T *dst, const S *src, int d = 1)
{
	for(int i = 0; i < N; ++i)
	{
		dst[i] = static_cast<T>(src[d*i]);
	}
}

}

template <typename T, int N>
struct vec
{
	/* Types */
	
	typedef T type;
	static const int size = N;
	
	/* Data */
	
	T data[N];
	
	template <typename S>
	void memcopy(const S *src, int d = 1)
	{
		__vec_tools__::memcopy<T,S,N>(data,src,d);
	}
	
	template <typename ... Args>
	void unroll(Args ... args)
	{
		__vec_tools__::Unroller<T,N>::unroll(data,args...);
	}

	/* Constructors */
	
	vec()
	{
		
	}
	
	template <typename ... Args>
	vec(Args ... args)
	{
		/* TODO: Initialize with vectors with less dimensions */
		unroll(args...);
	}
	
	/* Copy constructors */
	
	template <typename S>
	vec(const vec<S,N> &v)
	{
		memcopy(v.data);
	}
	
	/* Assign operators */
	
	template <typename S>
	vec &operator = (const vec<S,N> &v)
	{
		memcopy(v.data);
		return *this;
	}
	
	/* Access operators */
	
	T &operator [](int n)
	{
		return data[n];
	}
	
	T operator [](int n) const 
	{
		return data[n];
	}
	
	T &operator ()(int n) 
	{
		return data[n];
	}
	
	T operator ()(int n) const 
	{
		return data[n];
	}
	
		/* Index access */
	
	T x() const
	{
		return data[0];
	}
	
	T &x()
	{
		return data[0];
	}
	
	T y() const
	{
		return data[1];
	}
	
	T &y()
	{
		return data[1];
	}
	
	T z() const
	{
		return data[2];
	}
	
	T &z()
	{
		return data[2];
	}
	
	T w() const
	{
		return data[3];
	}
	
	T &w()
	{
		return data[3];
	}
};

/* Addition */

template<typename T, typename S, int N>
vec<typename std::common_type<T,S>::type,N> operator +(const vec<T,N> &a, const vec<S,N> &b) 
{
	vec<typename std::common_type<T,S>::type,N> c;
	for(int i = 0; i < N; ++i) 
	{
		c.data[i] = a.data[i] + b.data[i];
	}
	return c;
}

/* Multiplication by constant */

template<typename T, typename S, int N>
vec<typename std::common_type<T,S>::type,N> operator *(S a, const vec<T,N> &b) 
{
	vec<typename std::common_type<T,S>::type,N> c;
	for(int i = 0; i < N; ++i) 
	{
		c.data[i] = a*b.data[i];
	}
	return c;
}

/* Component product */

template<typename T, int N>
vec<T,N> operator &(const vec<T,N> &a, const vec<T,N> &b) 
{
  vec<T,N> c;
  for(int i = 0; i < N; ++i) 
  {
	  c.data[i] = a.data[i]*b.data[i];
  }
  return c;
}

/* Scalar product */

template<typename T, typename S, int N>
typename std::common_type<T,S>::type operator *(const vec<T,N> &a, const vec<S,N> &b) 
{
	T c = static_cast<typename std::common_type<T,S>::type>(0);
	for(int i = 0; i < N; ++i) 
	{
		c += a.data[i]*b.data[i];
	}
	return c;
}

/* Pseudo cross product */

template <typename T, typename S>
typename std::common_type<T,S>::type operator ^(const vec<T,2> &a, const vec<S,2> &b)
{
	return a[0]*b[1] - a[1]*b[0];
}

/* Cross product */

template <typename T, typename S>
vec<typename std::common_type<T,S>::type,3> operator ^(const vec<T,3> &a, const vec<S,3> &b) {
	return vec<typename std::common_type<T,S>::type,3>(
	  a[1]*b[2] - b[1]*a[2],
	  a[2]*b[0] - b[2]*a[0],
	  a[0]*b[1] - b[0]*a[1]
	);
}

/* Derivative operations */

template<typename T, typename S, int N>
vec<typename std::common_type<T,S>::type,N> operator *(const vec<T,N> &b, S a)
{
	return a*b;
}

template<typename T, int N>
vec<T,N> operator +(const vec<T,N> &a) 
{
	return a;
}

template<typename T, int N>
vec<T,N> operator -(const vec<T,N> &a) 
{
	return static_cast<T>(-1)*a;
}

template<typename T, typename S, int N>
vec<typename std::common_type<T,S>::type,N> operator -(const vec<T,N> &a, const vec<S,N> &b) 
{
	return a+(-b);
}

template<typename T, typename S, int N>
vec<typename std::common_type<T,S>::type,N> operator /(const vec<T,N> &b, S a) 
{
	return b*(static_cast<typename std::common_type<T,S>::type>(1)/static_cast<typename std::common_type<T,S>::type>(a));
}

/* Assign operations */

template<typename T, typename S, int N>
vec<T,N> &operator +=(vec<T,N> &a, const vec<S,N> &b) 
{
	return a = a + b;
}

template<typename T, typename S, int N>
vec<T,N> &operator -=(vec<T,N> &a, const vec<S,N> &b) 
{
	return a = a - b;
}

template<typename T, typename S, int N>
vec<T,N> &operator *=(vec<T,N> &a, S b) 
{
	return a = a*b;
}

template<typename T, typename S, int N>
vec<T,N> &operator /=(vec<T,N> &a, S b) 
{
	return a = a/b;
}

/* Math */

#include<cmath>

template<typename T, int N>
inline T sqr(const vec<T,N> &v) 
{
	return v*v;
}

template<typename T, int N>
inline T length(const vec<T,N> &v) 
{
	return sqrt(sqr(v));
}

template<typename T, int N>
inline vec<T,N> norm(const vec<T,N> &v) 
{
	return v/length(v);
}

/* Comparison */

template<typename T, int N>
inline bool operator ==(const vec<T,N> &a, const vec<T,N> &b) 
{
	for(int i = 0; i < N; ++i) 
	{
		if(a.data[i] != b.data[i]) 
		{
			return false;
		}
	}
	return true;
}

template<typename T, int N>
inline bool operator !=(const vec<T,N> &a, const vec<T,N> &b) 
{
	return !(a==b);
}

/* Type aliases and constants */

typedef vec<double,2> dvec2;
typedef vec<float,2>  fvec2;
typedef vec<int,2>    ivec2;

const dvec2 nulldvec2(0.0,0.0);
const fvec2 nullfvec2(0.0f,0.0f);
const ivec2 nullivec2(0,0);

typedef dvec2 vec2;
const vec2 nullvec2 = nulldvec2;

typedef vec<double,3> dvec3;
typedef vec<float,3>  fvec3;
typedef vec<int,3>    ivec3;

const dvec3 nulldvec3(0.0,0.0,0.0);
const fvec3 nullfvec3(0.0f,0.0f,0.0f);
const ivec3 nullivec3(0,0,0);

typedef dvec3 vec3;
const vec3 nullvec3 = nulldvec3;

typedef vec<double,4> dvec4;
typedef vec<float,4>  fvec4;
typedef vec<int,4>    ivec4;

const dvec4 nulldvec4(0.0,0.0,0.0,0.0);
const fvec4 nullfvec4(0.0f,0.0f,0.0f,0.0f);
const ivec4 nullivec4(0,0,0,0);

typedef dvec4 vec4;
const vec4 nullvec4 = nulldvec4;
