// -*- c++ -*- (enables emacs c++ mode)
//========================================================================
//
// Library : GEneric Tool for Finite Elements Methods (getfem)
// File    : getfem_mesh_im_level_set.cc : adaptable integration methods
//           on convex meshes.
// Date    : February 02, 2005.
// Author  : Yves Renard <Yves.Renard@insa-toulouse.fr>
//
//========================================================================
//
// Copyright (C) 2005-2005 Yves Renard
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

#include <getfem_mesh_im_level_set.h>
#include <getfem_mesher.h>
#include <bgeot_kdtree.h>



namespace getfem {
  static bool noisy = false;
  void getfem_mesh_im_level_set_noisy(void) { noisy = true; }

  
  void mesh_im_level_set::receipt(const MESH_CLEAR &) { clear(); }
  void mesh_im_level_set::receipt(const MESH_DELETE &) { clear(); }
  void mesh_im_level_set::clear(void) { mesh_im::clear(); level_sets.clear(); }

  mesh_im_level_set::mesh_im_level_set(getfem_mesh &me,
				       pintegration_method reg,
				       pintegration_method sing) 
    : mesh_im(me), cut_im(me) {
    regular_simplex_pim = reg;
    singular_simplex_pim = (sing == 0) ? reg : sing;
  }

  pintegration_method 
  mesh_im_level_set::int_method_of_element(size_type cv) const {
    if (cut_im.convex_index().is_in(cv)) 
      return cut_im.int_method_of_element(cv); 
    else return mesh_im::int_method_of_element(cv);
  }

  
  static void interpolate_face(getfem_mesh &m, dal::bit_vector& ptdone, 
			       const std::vector<size_type>& ipts,
			       bgeot::pconvex_structure cvs, 
			       size_type nb_vertices,
			       const std::vector<dal::bit_vector> &constraints,
			       const std::vector<const
			       mesher_signed_distance *> &list_constraints) {
    if (cvs->dim() == 0) return;
    else if (cvs->dim() > 1) {
      std::vector<size_type> fpts;
      for (size_type f=0; f < cvs->nb_faces(); ++f) {
	fpts.resize(cvs->nb_points_of_face(f));
	for (size_type k=0; k < fpts.size(); ++k)
	  fpts[k] = ipts[cvs->ind_points_of_face(f)[k]];
	interpolate_face(m,ptdone,fpts,cvs->faces_structure()[f], nb_vertices,
			 constraints, list_constraints);
      }
    }
    dal::bit_vector cts; size_type cnt = 0;
    for (size_type i=0; i < ipts.size(); ++i) {
      if (ipts[i] < nb_vertices) { 
	if (cnt == 0) cts = constraints[ipts[i]];
	else cts &= constraints[ipts[i]];
	++cnt;
      }
    }
    if (cts.card()) {
      // dal::bit_vector new_cts;
      for (size_type i=0; i < ipts.size(); ++i) {
	if (ipts[i] >= nb_vertices && !ptdone[ipts[i]]) { 
	  base_node &P = m.points()[ipts[i]];
	  pure_multi_constraint_projection(list_constraints, P, cts);
	  // dist(P, new_cts);
	}
      }
    }
  }


  struct point_stock {
    
    dal::dynamic_tree_sorted
    <base_node, dal::lexicographical_less<base_node,
		dal::approx_less<scalar_type> > > points;
    std::vector<dal::bit_vector> constraints_;
    std::vector<scalar_type> radius_;
    const std::vector<const mesher_signed_distance*> &list_constraints;
    
    void clear(void) { points.clear(); constraints_.clear();radius_.clear(); }
    scalar_type radius(size_type i) const { return radius_[i]; }
    const dal::bit_vector &constraints(size_type i) const
    { return constraints_[i]; }
    size_type size(void) const { return points.size(); }
    const base_node &operator[](size_type i) const { return points[i]; }

    size_type add(const base_node &pt, const dal::bit_vector &bv,
		    scalar_type r) {
      size_type j = points.search(pt);
      if (j == size_type(-1)) {
	j = points.add(pt);
	constraints_.push_back(bv);
	radius_.push_back(r);
      }
      return j;
    }
    size_type add(const base_node &pt, scalar_type r) {
      size_type j = points.search(pt);
      if (j == size_type(-1)) {
	dal::bit_vector bv;
	for (size_type i = 0; i < list_constraints.size(); ++i)
	  if (gmm::abs((*(list_constraints[i]))(pt)) < 1E-8) bv.add(i);
	j = points.add(pt);
	constraints_.push_back(bv);
	radius_.push_back(r);
      }
      return j;
    }
    size_type add(const base_node &pt) {
      size_type j = points.search(pt);
      if (j == size_type(-1)) {
	dal::bit_vector bv;
	for (size_type i = 0; i < list_constraints.size(); ++i)
	  if (gmm::abs((*(list_constraints[i]))(pt)) < 1E-8) bv.add(i);
	j = points.add(pt);
	constraints_.push_back(bv);
	scalar_type r = min_curvature_radius_estimate(list_constraints,pt,bv);
	radius_.push_back(r);
      }
      return j;
    }

    dal::bit_vector decimate(const mesher_signed_distance &ref_element,
			     scalar_type dmin) const {
      dal::bit_vector retained_points;
      if (size() != 0) {
	size_type n = points[0].size();

	bgeot::kdtree points_tree;
	points_tree.reserve(size());
	for (size_type i = 0; i < size(); ++i)
	  points_tree.add_point_with_id(points[i], i);

	for (size_type nb_co = n; nb_co != size_type(-1); nb_co --) {
	  for (size_type i = 0; i < size(); ++i) {
	    if (!(retained_points.is_in(i)) &&
		(constraints(i).card() == nb_co ||
		 (nb_co == n && constraints(i).card() > n)) &&
		ref_element(points[i]) < 1E-8) {
	      bool kept = true;
	      scalar_type d = (nb_co == 0) ? (dmin * 3.)
		: std::min(radius(i)*0.25, dmin);
	      // if (nb_co == n) d = dmin / 10.; // utile ?
	      base_node min = points[i], max = min;
	      for (size_type m = 0; m < n; ++m) { min[m]-=d; max[m]+=d; }
	      bgeot::kdtree_tab_type inpts;
	      points_tree.points_in_box(inpts, min, max);
	      for (size_type j = 0; j < inpts.size(); ++j)
		if (retained_points.is_in(inpts[j].i) &&
		    constraints(inpts[j].i).contains(constraints(i))
		    && gmm::vect_dist2(points[i], points[inpts[j].i]) < d)
		  { kept = false; break; }
	      if (kept) {
		if (noisy) cout << "kept point : " << points[i] << " co = "
				<< constraints(i) << endl;
		retained_points.add(i);
	      }
	    }
	  }
	}
      }
      return retained_points;
    }

    point_stock(const std::vector<const mesher_signed_distance*> &ls)
      : points(dal::lexicographical_less<base_node,
	       dal::approx_less<scalar_type> >(1E-7)), list_constraints(ls) {}
  };


  static mesher_signed_distance *new_ref_element(bgeot::pgeometric_trans pgt) {
    size_type n = pgt->structure()->dim();
    size_type nbp = pgt->basic_structure()->nb_points();
    /* Identifying simplexes.                                          */
    if (nbp == n+1 && pgt->basic_structure() == bgeot::simplex_structure(n)) {
	return new mesher_simplex_ref(n);
    }
    
    /* Identifying parallelepiped.                                     */
    if (nbp == (size_type(1) << n) &&
	pgt->basic_structure() == bgeot::parallelepiped_structure(n)) {
      base_node rmin(n), rmax(n);
      std::fill(rmax.begin(), rmax.end(), scalar_type(1));
      return new mesher_rectangle(rmin, rmax);
    }
    
    /* Identifying prisms.                                             */
    if (nbp == 2 * n && pgt->basic_structure() == bgeot::prism_structure(n)) {
      return new mesher_prism_ref(n);
    }
    
    DAL_THROW(to_be_done_error,
	      "This element is not taken into account. Contact us");
  }


  static getfem_mesh global_mesh; // to visualize the result with Open dx

  void mesh_im_level_set::cut_element(size_type cv,
				      const dal::bit_vector &primary,
				      const dal::bit_vector &secondary) {
    cout << "cutting element " << cv << endl;
    bgeot::pgeometric_trans pgt = linked_mesh().trans_of_convex(cv);
    std::auto_ptr<mesher_signed_distance> ref_element(new_ref_element(pgt));
    std::vector<mesher_level_set> mesher_level_sets;
    
    size_type n = pgt->structure()->dim();
    size_type nbtotls = primary.card() + secondary.card();
    pintegration_method exactint = classical_exact_im(pgt);
    if (nbtotls > 16)
      DAL_THROW(failure_error,
		"An element is cutted by more than 16 level_set, aborting");

    /* 
     * Step 1 : build the signed distances, estimate the curvature radius
     *          and the degree K.
     */
    dim_type K = 0; // Max. degree of the level sets.
    scalar_type r0 = 1E+10; // min curvature radius
    std::vector<const mesher_signed_distance*> list_constraints;
    point_stock mesh_points(list_constraints);

    ref_element->register_constraints(list_constraints);
    mesher_level_sets.reserve(nbtotls);
    size_type ll = 0;
    for (std::set<plevel_set>::const_iterator it = level_sets.begin();
	 it != level_sets.end(); ++it, ++ll) {
      if (primary[ll]) {
	base_node X(n);
	K = std::max(K, (*it)->degree());
	mesher_level_sets.push_back((*it)->mls_of_convex(cv, 0));
	mesher_level_set &mls(mesher_level_sets.back());
	list_constraints.push_back(&mesher_level_sets.back());
	r0 = std::min(r0, curvature_radius_estimate(mls, X, true));
	if (secondary[ll]) {
	  mesher_level_sets.push_back((*it)->mls_of_convex(cv, 1));
	  mesher_level_set &mls2(mesher_level_sets.back());
	  list_constraints.push_back(&mesher_level_sets.back());
	  r0 = std::min(r0, curvature_radius_estimate(mls2, X, true));
	}
      }
    }
    
    /* 
     * Step 2 : projection of points of a grid on each signed distance.
     */
    scalar_type h0 = std::min(1./(K+1), 0.2 * r0), dmin = 0.55;
    bool h0_is_ok = true;

    do {
      h0_is_ok = true;
      mesh_points.clear();
      scalar_type geps = .001*h0; 
      size_type nbpt = 1;
      std::vector<size_type> gridnx(n);
      for (size_type i=0; i < n; ++i)
	{ gridnx[i]=1+(size_type)(1./h0); nbpt *= gridnx[i]; }
      base_node P(n), Q(n), V(n);
      dal::bit_vector co;
      std::vector<size_type> co_v;

      for (size_type i=0; i < nbpt; ++i) {
	for (size_type k=0, r = i; k < n; ++k) { // building grid point
	  unsigned p =  r % gridnx[k];
	  P[k] = p * 1. / (gridnx[k]-1);
	  r /= gridnx[k];
	}
	co.clear(); co_v.resize(0);
	if ((*ref_element)(P) < geps) {
	  mesh_points.add(P, 1.E10);
	  for (size_type k=0; k < list_constraints.size(); ++k) {
	    gmm::copy(P, Q);
	    scalar_type d = list_constraints[k]->grad(P, V);
	    if (gmm::vect_norm2(V)*h0 > gmm::abs(d))
	      if (try_projection(*(list_constraints[k]), Q, true))
		if (gmm::vect_dist2(P, Q) < h0 / 1.5) {
		  co.add(k); co_v.push_back(k); 
		  if ((*ref_element)(Q) < 1E-8) {
		    scalar_type r =
		      curvature_radius_estimate(*(list_constraints[k]), Q);
		    r0 = std::min(r0, r);
		    if (r0 < 4.*h0) { h0 = 0.2 * r0; h0_is_ok = false; break; }
		    mesh_points.add(Q, r);
		  }
		}
	  }
	}
	size_type nb_co = co.card();
	dal::bit_vector nn;
	if (h0_is_ok)
	  for (size_type count = 0; count < (size_type(1) << nb_co); ++count) {
	    nn.clear();
	    for (size_type j = 0; j < nb_co; ++j)
	      if (count & (size_type(1) << j)) nn.add(co_v[j]);
	    if (nn.card() > 1 && nn.card() <= n) {
	      if (noisy) cout << "trying set of constraints" << nn << endl;
	      gmm::copy(P, Q);
	      bool ok=pure_multi_constraint_projection(list_constraints,Q,nn);
	      if (ok && (*ref_element)(Q) < 1E-8) {
		if (noisy) cout << "Intersection point found " << Q << " with "
		     << nn << endl;
		mesh_points.add(Q);
	      }
	    }
	  }
      }
      
      if (!h0_is_ok) continue;
      
    /* 
     * Step 3 : Delaunay, test if a simplex cut a level set and adapt
     *          the mesh to the curved level sets.
     */

      dmin = 2.*h0;
      if (noisy) cout << "dmin = " << dmin << " h0 = " << h0 << endl;
      if (noisy) cout << "cetait le convexe " << cv << endl;
      if (noisy) getchar(); 
      
      dal::bit_vector retained_points
	= mesh_points.decimate(*ref_element, dmin);
      
    delaunay_again :
      std::vector<base_node> fixed_points;
      std::vector<dal::bit_vector> fixed_points_constraints;
      fixed_points.reserve(retained_points.card());
      fixed_points_constraints.reserve(retained_points.card());
      for (dal::bv_visitor i(retained_points); !i.finished(); ++i) {
	fixed_points.push_back(mesh_points[i]);
	fixed_points_constraints.push_back(mesh_points.constraints(i));
      }
      gmm::dense_matrix<size_type> simplexes;
      delaunay(fixed_points, simplexes);
      if (noisy) cout << "Nb simplexes = " << gmm::mat_ncols(simplexes)<< endl;
      getfem_mesh mesh;
      for (size_type i = 0; i <  fixed_points.size(); ++i) {
	size_type j = mesh.add_point(fixed_points[i], false);
	assert(j == i);
      }
      std::vector<base_node> cvpts;
      for (size_type i = 0; i < gmm::mat_ncols(simplexes); ++i) {
	size_type j = mesh.add_convex(bgeot::simplex_geotrans(n,1),
		         gmm::vect_const_begin(gmm::mat_col(simplexes, i)));
	if (mesh.convex_quality_estimate(j) < 1E-18) mesh.sup_convex(j);
	else {
	  std::vector<scalar_type> signs(list_constraints.size());
	  std::vector<size_type> prev_point(list_constraints.size());
	  for (size_type ii = 0; ii <= n; ++ii) {
	    for (size_type jj = 0; jj < list_constraints.size(); ++jj) {
	      scalar_type dd =
		(*(list_constraints[jj]))(mesh.points_of_convex(j)[ii]);
	      if (gmm::abs(dd) > 1E-7) {
		if (dd * signs[jj] < 0.0) {
		  if (noisy) cout << "Intersection trouvee ... \n";
		  // calcul d'intersection
		  base_node X = mesh.points_of_convex(j)[ii], G;
		  base_node VV = mesh.points_of_convex(j)[prev_point[jj]] - X;
		  if (dd > 0.) gmm::scale(VV, -1.);
		  dd = (*(list_constraints[jj])).grad(X, G);
		  size_type nbit = 0;
		  while (gmm::abs(dd) > 1e-15) {
		    if (++nbit > 1000) {
		      if (noisy) cout << "Intersection not found";
		      assert(false);
		    }
		    scalar_type nG = std::max(1E-8, gmm::vect_sp(G, VV));
		    gmm::add(gmm::scaled(VV, -dd / nG), X);
		    dd = (*(list_constraints[jj])).grad(X, G);
		  }
		  size_type kk = mesh_points.add(X);
		  if (!(retained_points[kk])) {
		    retained_points.add(kk);
		    goto delaunay_again;
		  }
		}
		if (signs[jj] == 0.0) { signs[jj] = dd; prev_point[jj] = ii; }
	      }
	    }
	  }
	  if (K > 1) {
	    bgeot::pgeometric_trans pgt2 = bgeot::simplex_geotrans(n, K);
	    cvpts.resize(pgt2->nb_points());
	    for (size_type k=0; k < pgt2->nb_points(); ++k) {
	      cvpts[k] = bgeot::simplex_geotrans(n,1)->transform
		(pgt2->convex_ref()->points()[k], mesh.points_of_convex(j));
	    }
	    mesh.sup_convex(j);
	    mesh.add_convex_by_points(pgt2, cvpts.begin());
	  }
	}
      }
      
      if (noisy) {
	getfem::stored_mesh_slice sl;
	sl.build(mesh, getfem::slicer_none(), 1);
	char s[512]; sprintf(s, "totobefore%d.dx", cv);
	getfem::dx_export exp(s);
	exp.exporting(sl);
	exp.exporting_mesh_edges();
	exp.write_mesh();
      }
      
      if (K > 1) { // � ne faire que sur les convexes concern�s ..
	dal::bit_vector ptdone;
	std::vector<size_type> ipts;
	for (dal::bv_visitor i(mesh.convex_index()); !i.finished(); ++i) {
	  for (size_type f = 0; f <= n; ++f) {
	    bgeot::ind_ref_mesh_point_ind_ct fpts
	      = mesh.ind_points_of_face_of_convex(i, f);
	    ipts.assign(fpts.begin(), fpts.end());
	    interpolate_face(mesh, ptdone, ipts,
			     mesh.trans_of_convex(i)->structure()
			     ->faces_structure()[f], fixed_points.size(),
			     fixed_points_constraints, list_constraints);
	  }
	}
      }
      
      if (noisy) {
	getfem::stored_mesh_slice sl;
	sl.build(mesh, getfem::slicer_none(), 12);
	char s[512]; sprintf(s, "toto%d.dx", cv);
	getfem::dx_export exp(s);
	exp.exporting(sl);
	exp.exporting_mesh_edges();
	exp.write_mesh();
      }
      
      std::vector<base_node> gauss_points;
      std::vector<scalar_type> gauss_weights;
      for (dal::bv_visitor i(mesh.convex_index()); !i.finished(); ++i) {
	base_matrix G;
	vectors_to_base_matrix(G, mesh.points_of_convex(i));
	
	papprox_integration pai = regular_simplex_pim->approx_method();
	bgeot::geotrans_interpolation_context c(mesh.trans_of_convex(i),
						pai->point(0), G);
	for (size_type j = 0; j < pai->nb_points_on_convex(); ++j) {
	  c.set_xref(pai->point(j));
	  gauss_points.push_back(c.xreal());
	  gauss_weights.push_back(pai->coeff(j) * gmm::abs(c.J()));
	}
      }
      
      // + test ...
      scalar_type wtot(0);
      if (noisy) cout << "Number of gauss points : " << gauss_weights.size();
      for (size_type k = 0; k < gauss_weights.size(); ++k)
	wtot += gauss_weights[k];
      base_poly poly = bgeot::one_poly(n);
      scalar_type exactvalue = exactint->exact_method()->int_poly(poly);
      if (noisy) cout.precision(16);
      if (noisy) cout << "The Result : " << wtot << " compared to "
		      << exactvalue << endl; 
      
      if (gmm::abs(wtot-exactvalue) > 1E-7){
	if (noisy) cout << "PAS BON NON PLUS\n"; if (noisy) getchar();
	if (dmin > 3*h0) { dmin /= 2.; }
	else { h0 /= 2.0; dmin = 2.*h0; }
	h0_is_ok = false;
      }
 
      if (h0_is_ok && noisy) { // ajout dans global mesh pour visu
	base_matrix G;
	vectors_to_base_matrix(G, linked_mesh().points_of_convex(cv));
	std::vector<size_type> pts(mesh.nb_points());
	for (size_type i = 0; i < mesh.nb_points(); ++i)
	  pts[i] = global_mesh.add_point(pgt->transform(mesh.points()[i], G));
	
	for (dal::bv_visitor i(mesh.convex_index()); !i.finished(); ++i)
	  global_mesh.add_convex(mesh.trans_of_convex(i), 
				 dal::index_ref_iterator(pts.begin(),
				     mesh.ind_points_of_convex(i).begin()));
      }

    } while (!h0_is_ok);
  }


  void mesh_im_level_set::adapt(void) {

    // compute the elements touched by each level set
    // for each element touched, compute the sub mesh
    //   then compute the adapted integration method
    for (dal::bv_visitor cv(linked_mesh().convex_index()); 
	 !cv.finished(); ++cv) {
      dal::bit_vector prim, sec;
      find_crossing_level_set(cv, prim, sec);
      if (noisy) cout << "element " << cv << " cutted level sets : "
		      << prim << endl;
      if (prim.card()) cut_element(cv, prim, sec);
    }
    getfem::stored_mesh_slice sl;
    sl.build(global_mesh, getfem::slicer_none(), 6);
    getfem::dx_export exp("totoglob.dx");
    exp.exporting(sl);
    exp.exporting_mesh_edges();
    exp.write_mesh();

  }

  bool mesh_im_level_set::is_crossed_by(size_type cv, plevel_set ls,
					unsigned lsnum) {
    const mesh_fem &mf = ls->get_mesh_fem();
    bgeot::pgeometric_trans pgt = linked_mesh().trans_of_convex(cv);

    ref_mesh_dof_ind_ct dofs = mf.ind_dof_of_element(cv);
    pfem pf = mf.fem_of_element(cv);
    int p = -2;
    mesher_level_set mls1 = ls->mls_of_convex(cv, lsnum, false);
    scalar_type EPS = 1e-8;

    /* easy cases: 
     - different sign on the dof nodes => intersection for sure
     - min value of the levelset greater than the radius of the convex
     => no intersection
    */ 
    for (ref_mesh_dof_ind_ct::const_iterator it=dofs.begin();
	 it != dofs.end(); ++it) {
      scalar_type v = ls->values()[*it];
      int p2 = ( (v < -EPS) ? -1 : ((v > EPS) ? +1 : 0));
      if (p == -2) p=p2; else if (p*p2 < 0) return true;
    }

    base_node X(pf->dim()), G(pf->dim());
    gmm::fill_random(X); X *= 1E-2;
    scalar_type d = mls1.grad(X, G);
    if (gmm::vect_norm2(G)*2. < gmm::abs(d)) return false;

    std::auto_ptr<mesher_signed_distance> ref_element(new_ref_element(pgt));
    
    gmm::fill_random(X); X *= 1E-2;
    mesher_intersection mi1(*ref_element, mls1);
    if (!try_projection(mi1, X)) return false;
    if ((*ref_element)(X) > 1E-8) return false;
    
    gmm::fill_random(X); X *= 1E-2;
    mesher_level_set mls2 = ls->mls_of_convex(cv, lsnum, true);
    mesher_intersection mi2(*ref_element, mls2);
    if (!try_projection(mi2, X)) return false;
    if ((*ref_element)(X) > 1E-8) return false;
   
    return true;
  }

  void mesh_im_level_set::find_crossing_level_set(size_type cv,
						  dal::bit_vector &prim,
						  dal::bit_vector &sec) {
    prim.clear(); sec.clear();
    unsigned lsnum = 0, k = 0;
    for (std::set<plevel_set>::const_iterator it = level_sets.begin(); 
	 it != level_sets.end(); ++it, ++lsnum, ++k) {
      if (noisy) cout << "testing cv " << cv << " with level set "
		      << k << endl;
      if (is_crossed_by(cv, *it, 0)) {
	prim.add(lsnum);
	if ((*it)->has_secondary() && is_crossed_by(cv, *it, 1))
	  sec.add(lsnum);
      }
    }
  }


  
}  /* end of namespace getfem.                                             */



