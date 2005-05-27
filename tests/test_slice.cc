#include <bgeot_comma_init.h>
#include <getfem_mesh_slice.h>
#ifdef GETFEM_HAVE_FEENABLEEXCEPT
#  include <fenv.h>
#endif

using getfem::size_type;
namespace getfem {
  std::ostream& operator<<(std::ostream& o, const bgeot::mesh_structure& m) {
    o << "mesh_structure: nb_pts=" << m.point_structures().size() << ", nb_cvs=" << m.convex_index().card() << endl;
    dal::bit_vector bv = m.convex_index();
    for (size_type cv = bv.take_first(); cv != size_type(-1); cv << bv) {
      o << "convex " << cv << ": "; 
      std::copy(m.ind_points_of_convex(cv).begin(), m.ind_points_of_convex(cv).end(), std::ostream_iterator<size_type>(o, ","));
      o << endl;
    }
    return o;
  }

#if 0  
  std::ostream& operator<<(std::ostream& o, const mesh_slice& m) {
    std::vector<size_type> e;
    o << "mesh_slice, containing " << m.nb_convex() << " convexes\n";
    for (size_type ic = 0; ic < m.nb_convex(); ++ic) {
      o << "slice convex #" << ic << " (original = " << m.convex_num(ic) << ")\n";
      for (size_type i = 0; i < m.nodes(ic).size(); ++i) {
        o << "node " << i << ": " << m.nodes(ic)[i].pt << ", ref=" << m.nodes(ic)[i].pt_ref << " flist=" << m.nodes(ic)[i].faces << endl;
      }
      for (size_type i = 0; i < m.simplexes(ic).size(); ++i) {
        o << "simplex " << i << ", inodes=";
        for (size_type j=0;j< m.simplexes(ic)[i].dim()+1;++j)
          o << m.simplexes(ic)[i].inodes[j] << " ";
        o << endl;
      }
      /*m.edges(ic,e);
      o << "edges: "; for (size_type i=0; i < e.size()/2; ++i) o << e[2*i] << "-" << e[2*i+1] << " ";
      o << endl;
      */
    }
    return o;
  }
#endif
}

int 
main() {
#ifdef GETFEM_HAVE_FEENABLEEXCEPT /* trap SIGFPE */
  feenableexcept(FE_DIVBYZERO | FE_INVALID);
#endif
  getfem::getfem_mesh m;
  getfem::base_node A; bgeot::sc(A)=0,0;
  getfem::base_node B; bgeot::sc(B)=1,0;
  getfem::base_node C; bgeot::sc(C)=0,2;
  getfem::base_node D; bgeot::sc(D)=1,1;
  m.add_triangle_by_points(A,B,C);
  m.add_triangle_by_points(B,C,D);

  getfem::base_node x0; bgeot::sc(x0) = .4,0;
  getfem::base_node n0;  bgeot::sc(n0) = 1,0;  
  getfem::base_node x1; bgeot::sc(x1) = 0,0.1;
  getfem::base_node n1;  bgeot::sc(n1) = 1,-1;  

  getfem::mesh_region cvlst; 
  cvlst.add(0);
  cvlst.add(1);
  cout << "ok\n";
  getfem::stored_mesh_slice sl; sl.build(m, getfem::slicer_half_space(x0,n0,false), 10);
  cout << sl << endl;
  cout << "memory 0: " << sl.memsize() << " bytes\n";

  sl.clear();
  getfem::slicer_half_space slh1(x1,n1,false);
  getfem::mesh_slicer ms1(m); 
  ms1.push_back_action(slh1); 
  getfem::slicer_build_stored_mesh_slice slb(sl);
  ms1.push_back_action(slb);

  ms1.exec(10,cvlst);
  cout << sl << endl;

  cout << "memory 1: " << sl.memsize() << " bytes\n";
  return 0;
}
