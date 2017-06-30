/* -*- c++ -*- (enables emacs c++ mode) */
/*===========================================================================

 Copyright (C) 2000-2017 Julien Pommier

 This file is a part of GetFEM++

 GetFEM++  is  free software;  you  can  redistribute  it  and/or modify it
 under  the  terms  of the  GNU  Lesser General Public License as published
 by  the  Free Software Foundation;  either version 3 of the License,  or
 (at your option) any later version along with the GCC Runtime Library
 Exception either version 3.1 or (at your option) any later version.
 This program  is  distributed  in  the  hope  that it will be useful,  but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or  FITNESS  FOR  A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License and GCC Runtime Library Exception for more details.
 You  should  have received a copy of the GNU Lesser General Public License
 along  with  this program;  if not, write to the Free Software Foundation,
 Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.

 As a special exception, you  may use  this file  as it is a part of a free
 software  library  without  restriction.  Specifically,  if   other  files
 instantiate  templates  or  use macros or inline functions from this file,
 or  you compile this  file  and  link  it  with other files  to produce an
 executable, this file  does  not  by itself cause the resulting executable
 to be covered  by the GNU Lesser General Public License.  This   exception
 does not  however  invalidate  any  other  reasons why the executable file
 might be covered by the GNU Lesser General Public License.

===========================================================================*/

/**@file getfem_import.h
   @author  Julien Pommier <Julien.Pommier@insa-toulouse.fr>
   @date Januar 17, 2003.
   @brief Import mesh files from various formats.
*/

#ifndef GETFEM_IMPORT_H__
#define GETFEM_IMPORT_H__

#include <string>
#include <iostream>
#include <map>
#include <set>
#include "bgeot_config.h" /* for bgeot::size_type */

namespace getfem {
  using bgeot::size_type;
  class mesh;

  /** imports a mesh file.
      format can be:

      - "gid" for meshes generated by GiD http://gid.cimne.upc.es/
         -- mesh nodes are always 3D


      - "gmsh" for meshes generated by Gmsh http://www.geuz.org/gmsh/
       IMPORTANT NOTE: if you do not assign a physical surface/volume
       to your 3D mesh, the file will also contain the mesh of the
       boundary (2D elements) and the boundary of the boundary (line
       elements!).

       GetFEM++ makes use of the physical "region" number stored with
       each element in the gmsh file to fill the corresponding region
       of the mesh object.

       For a mesh of dimension N, getfem builds a mesh with the
       convexes listed in the gmsh file whose dimension are N, the
       convexes of dim N-1 are used to tag "region" of faces,
       according to their gmsh "physical region number" (physical
       region number 'n' is mapped to the getfem region), and
       the convexes of lower dimension are ignored.

       All regions must have different number! This means that the
       parametrization of the mesh in Gmsh .geo file must assign a
       different number to each region, the problem exists because in
       Gmsh can coexist, for example, "Physical Surface (200)" and
       "Physical Line (200)", as they are different "types of regions"
       in Gmsh, that which does not occur in GetFEM++ since there is
       only one "type of region".


      - "cdb" for meshes generated by ANSYS (in blocked format).

       Currently, plane and solid elements of types 42,45,73,82,87,89,
       90,92,95,162,182,183,185,186,187 and 191 are supported.
       This however does not include any finite element techology linked
       to these elements but only their geometry.

       By default GetFEM++ will define mesh regions corresponding to
       material ids found in the imported cdb file, if there are more
       than one material ids available.

       Using "cdb:N" as format specifier, only elements with material
       id equal to an integer N will be imported.

       The recommended ANSYS command for generating a mesh file is:

       cdwrite,db,filename,cdb


      - "am_fmt" for 2D meshes from emc2
        [http://pauillac.inria.fr/cdrom/prog/unix/emc2/eng.htm]

  */
  void import_mesh(const std::string& filename, const std::string& format,
		   mesh& m);
  void import_mesh(std::istream& f, const std::string& format,
		   mesh& m);
  void import_mesh(const std::string& filename, mesh& m);

  /** Import a mesh file in format generated by Gmsh.

      The function works exactly like impot_mesh() functions
      except that they return additional mapping of physical
      region names to their numbers.

      The example below shows how to print region names
      and their numbers:
      @code
        getfem::mesh myMesh;
        typedef std::map<std::string, size_type> RegMap;
        typedef RegMap::iterator RegMapIter;
        RegMap regmap;
        getfem::import_mesh_gmsh("mesh.msh", myMesh, regmap);
        std::cout << regmap.size() << "\n";
        for (RegMapIter i=regmap.begin(); i != regmap.end(); i++) {
           std::cout << i->first << " " << i->second << "\n";
        }
      @endcode

      Additionally, the optional lower_dim_convex_rg defines a set
      of face regions that need to be imported explicitly as convexes.

      add_all_element_type flag, if set to true, will import all the lower
      dimension elements defined as independent convexes, only if the elements
      are not face of another convex. Thus, a 3D model can have a mixture of
      3D solid, 2D plates and 1D rod elements. This feature is still yet to
      be tested.
  */
  void import_mesh_gmsh(const std::string& filename, mesh& m,
                        std::map<std::string, size_type> &region_map,
                        bool remove_last_dimension = true,
                        std::map<size_type, std::set<size_type>> *nodal_map = NULL,
                        bool remove_duplicated_nodes = true);

  void import_mesh_gmsh(std::istream& f, mesh& m,
                        std::map<std::string, size_type> &region_map,
                        bool remove_last_dimension = true,
                        std::map<size_type, std::set<size_type>> *nodal_map = NULL,
                        bool remove_duplicated_nodes = true);

  void import_mesh_gmsh(const std::string& filename, mesh& m,
                        bool add_all_element_type = false,
                        std::set<size_type> *lower_dim_convex_rg = NULL,
                        std::map<std::string, size_type> *region_map = NULL,
                        bool remove_last_dimension = true,
                        std::map<size_type, std::set<size_type>> *nodal_map = NULL,
                        bool remove_duplicated_nodes = true);

  void import_mesh_gmsh(std::istream& f, mesh& m,
                        bool add_all_element_type = false,
                        std::set<size_type> *lower_dim_convex_rg = NULL,
                        std::map<std::string, size_type> *region_map = NULL,
                        bool remove_last_dimension = true,
                        std::map<size_type, std::set<size_type>> *nodal_map = NULL,
                        bool remove_duplicated_nodes = true);

  /** for gmsh and gid meshes, the mesh nodes are always 3D, so for a 2D mesh
      the z-component of nodes should be removed */
  void maybe_remove_last_dimension(mesh &msh);

  /** for gmsh meshes, create table linking region name to region_id. */
  std::map<std::string, size_type> read_region_names_from_gmsh_mesh_file(std::istream& f);
}
#endif /* GETFEM_IMPORT_H__  */
