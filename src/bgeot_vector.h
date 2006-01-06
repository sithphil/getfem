// -*- c++ -*- (enables emacs c++ mode)
//========================================================================
//
// Library : Basic GEOmetric Tool  (bgeot)
// File    : bgeot_vector.h : plain vectors.
//           
// Date    : June 01, 1995.
// Author  : Yves Renard <Yves.Renard@insa-toulouse.fr>
//
//========================================================================
//
// Copyright (C) 1995-2006 Yves Renard
//
// This file is a part of GETFEM++
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//========================================================================

/**@file bgeot_vector.h
   @brief some (old) vector class, and some typedefs.

   vsvector should be replaced by gmm calls.
*/

#ifndef BGEOT_VECTOR_H__
#define BGEOT_VECTOR_H__

#include <bgeot_config.h>
#include <gmm_kernel.h>
#include <bgeot_small_vector.h>

namespace bgeot
{

  /* ******************************************************************** */
  /*		Class vsvector<T>: variable size vector.     		  */
  /* ******************************************************************** */

  /** Plain vectors of arbitrary base type
   * ($<$T$>$) and with arbitrary number of components.
   *  It as been build especially for small vectors.
   * The copy operator is a true copy (i.e. no "sharing" copy).
   * The classes is directly derived from std::vector, with additional
   * classical linear algebraic operations.
   */
  template<class T> class vsvector : public std::vector<T>
  {
    public:

    typedef typename std::vector<T>::size_type size_type;
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

      void out_of_range_error(void) const;

      #ifdef GETFEM_VERIFY
      inline const T& operator [](size_type l) const
    { if (l>=this->size()) out_of_range_error(); return *(this->begin()+l); }
      inline T& operator [](size_type l)
    { if (l>=this->size()) out_of_range_error(); return *(this->begin()+l); }
      #endif

      void fill(const T &);
      void addmul(const T &, const vsvector<T>&)  IS_DEPRECATED;

      /// Add vector w to current vector.
      vsvector<T>& operator +=(const vsvector<T>& w);
      /// Substract vector w to current vector v.
      vsvector<T>& operator -=(const vsvector<T>& w);
      /// Multiply the current vector with the scalar x.
      vsvector<T>& operator *=(const T &x);
      /// Divide the current vector with the scalar x.
      vsvector<T>& operator /=(const T &x);
    
      /// Constructor. For 2 components initialized with a0 and a1.
      vsvector(T a0, T a1);
      /// Constructor. For 3 components initialized with a0, a1 and a2.
      vsvector(T a0, T a1, T a2);
      /// Constructor. For 4 components initialized with a0, a1, a2 and a3.
      vsvector(T a0, T a1, T a2, T a3);
      /// Constructor. A vector with l components.
      vsvector(size_type l) : std::vector<T>(l) {}
      /// Constructor.
      vsvector(void) : std::vector<T>() {}

    size_type memsize() const
      { return std::vector<T>::capacity()*sizeof(T) + sizeof(vsvector<T>); }
  };

  template<class T> vsvector<T>::vsvector(T a0, T a1)
    : std::vector<T>(size_type(2))
  { typename vsvector<T>::iterator p = this->begin(); *p++ = a0; *p++ = a1; }
  template<class T> vsvector<T>::vsvector(T a0, T a1, T a2)
    : std::vector<T>(size_type(3)) 
  { iterator p = this->begin(); *p++ = a0; *p++ = a1; *p++ = a2;}
  template<class T> vsvector<T>::vsvector(T a0, T a1, T a2, T a3)
    : std::vector<T>(size_type(4))
  { iterator p = this->begin(); *p++ = a0; *p++ = a1; *p++ = a2; *p++ = a3;}
  

  template<class T>  void vsvector<T>::out_of_range_error(void) const
  { DAL_THROW(std::out_of_range, "out of range"); }

  /*
    deprecated ?
  */
  template<class T>  void vsvector<T>::addmul(const T &a, const vsvector<T> &v)
  { 
    typename vsvector<T>::iterator d1 = this->begin(), e = this->end();
    const_iterator d2 = v.begin();
    if ( v.size() != this->size())
      DAL_THROW(dimension_error, "dimensions mismatch");
    while (d1 != e) *d1++ += (*d2++) * a;
  }

  template<class T>  void vsvector<T>::fill(const T &x)
  { std::fill(this->begin(), this->end(), x); }
 
  template<class T>  vsvector<T>& vsvector<T>::operator *=(const T &x)
  { gmm::scale(*this, x); return *this; }

  template<class T>  vsvector<T>& vsvector<T>::operator /=(const T &x)
  { gmm::scale(*this, T(1) / x); return *this; }

  template<class T> vsvector<T>& vsvector<T>::operator +=(const vsvector<T>& w)
  { gmm::add(w, *this); return *this; }

  template<class T> vsvector<T>& vsvector<T>::operator -=(const vsvector<T>& w)
  { gmm::add(gmm::scaled(w, T(-1)), *this); return *this; }

  template<class T> inline
  vsvector<T> operator *(const vsvector<T>& m, const T &x)
  { vsvector<T> p = m; p *= x; return p; }

  template<class T> inline vsvector<T> operator *(T x, const vsvector<T>& m)
  { vsvector<T> p = m; p *= x; return p; }

  template<class T> inline
  vsvector<T> operator /(const vsvector<T>& m, const T &x)
  { vsvector<T> p = m; p /= x; return p; }
  
  template<class T>
    inline vsvector<T> operator +(const vsvector<T>& m, const vsvector<T>& n)
  { vsvector<T> p = m; p += n; return p; }
  
  template<class T>
    inline vsvector<T> operator -(const vsvector<T>& m, const vsvector<T>& n)
  { vsvector<T> p = m; p -= n; return p; }

  template<class T> inline vsvector<T> operator -(const vsvector<T>& m)
  { vsvector<T> p = m; p *= T(-1); return p; }

  template<class T> inline vsvector<T> operator +(const vsvector<T>& p)
  { return p; }

  using gmm::vect_sp;
  using gmm::vect_norm1;
  using gmm::vect_norm2;
  using gmm::vect_norm2_sqr;

  /// Gives @f$(\sum_{i=0..(n-1)} |v_i - w_i|^2)@f$.
  template<class VEC> double vect_dist2_sqr(const VEC &v, const VEC &w)
  {
    typename VEC::const_iterator d1 = v.begin(), e = v.end();
    typename VEC::const_iterator d2 = w.begin();
    double res = 0;
    if (v.size() != w.size()) DAL_THROW(dimension_error,"dimensions mismatch");
    while (d1 != e) res += gmm::sqr((double)gmm::abs(*d1++ - *d2++));
    return res;
  }

  /// Gives @f$(\sum_{i=0..(n-1)} |v_i - w_i|^2)^{1/2}@f$.
  template<class VEC> double vect_dist2(const VEC &v, const VEC &w) {
    return sqrt(vect_dist2_sqr(v,w));
  }


  /* ******************************************************************** */
  /*		Points.                                     		  */
  /* ******************************************************************** */


  /** For any class of vector VECT, the class PT$<$VECT$>$ 
   *  represent the corresponding class of point.
   */
  template<class VECT> class PT : public VECT
  {
    public :
      typedef VECT vector_type;
      typedef typename VECT::value_type value_type;
      /// Constructor.
      PT(const VECT &v) : VECT(v) { }
      /// Constructor.
      PT(const PT &pt, const VECT &v) : VECT(pt)  { *this += v; }
      /// Constructor.
      PT(void) : VECT() {}
      /// Constructor.
      PT(int n) : VECT(n) {}

      PT &operator *=(const value_type &a)
      { *((VECT *)this) *= a; return *this; }
      PT &operator /=(const value_type &a)
      { *((VECT *)this) /= a; return *this; }
      PT &operator +=(const PT &v) { *((VECT *)this) += v; return *this; }
      PT &operator -=(const PT &v) { *((VECT *)this) -= v; return *this; }
  };

  /** @name generic functions on points
   */
  //@{

  /// Return the vector AB, where A and B are some points.
  template<class PT> typename PT::vector_type vector_from(const PT &A,
							  const PT &B)
  { typename PT::vector_type v = B; v -= A; return v; }

  /// Compute in v the vector AB, where A and B are some points.
  template<class PT> void vector_from(const PT &A,
				      const PT &B, typename PT::vector_type &v)
  { v = B; v -= A; }

  
  //@}

  typedef vsvector<scalar_type> base_vector;
  typedef small_vector<scalar_type> base_small_vector;
  //typedef vsvector<scalar_type> base_small_vector;
  /// type used for points of any (reasonnably small) dimension.
  typedef base_small_vector base_node;

  /*  template<class T>
    vsvector<T> operator *(const gmm::dense_matrix<T>& m, const vsvector<T>& v)
  { vsvector<T> res(m.nrows()); gmm::mult(m, v, res); return res; }

  template<class T>
  gmm::dense_matrix<T> operator *(const gmm::dense_matrix<T>& m,
				  const gmm::dense_matrix<T>& n) {
    gmm::dense_matrix<T> res(m.nrows(), n.ncols()); 
    gmm::mult(m, n, res);
    return res;
  }
  */
  typedef gmm::dense_matrix<scalar_type> base_matrix;

  template <class VEC_CONT> void vectors_to_base_matrix(base_matrix &G, const VEC_CONT &a) {
    size_type P = (*(a.begin())).size(), NP = a.end() - a.begin();
    G.resize(P, NP);
    typename VEC_CONT::const_iterator it = a.begin(), ite = a.end();
    base_matrix::iterator itm = G.begin();
    for (; it != ite; ++it, itm += P)
      std::copy((*it).begin(), (*it).end(), itm);
  }
}  /* end of namespace bgeot.                                           */

namespace std {
  inline void swap(bgeot::base_node& a, bgeot::base_node& b) { a.swap(b); }
}

#include <gmm_interface_bgeot.h>

#endif  /* BGEOT_VECTOR_H__ */
