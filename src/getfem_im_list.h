// This file is generated by make_getfem_list

/* *********************************************************************** */
/*                                                                         */
/* Copyright (C) 2000-2003  Yves Renard.                                   */
/*                                                                         */
/* This file is a part of GETFEM++                                         */
/*                                                                         */
/* This program is free software; you can redistribute it and/or modify    */
/* it under the terms of the GNU Lesser General Public License as          */
/* published by the Free Software Foundation; version 2.1 of the License.  */
/*                                                                         */
/* This program is distributed in the hope that it will be useful,         */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/* GNU Lesser General Public License for more details.                     */
/*                                                                         */
/* You should have received a copy of the GNU Lesser General Public        */
/* License along with this program; if not, write to the Free Software     */
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,  */
/* USA.                                                                    */
/*                                                                         */
/* *********************************************************************** */



namespace getfem {


  struct im_desc {
      const char *method_name;
      const char *geotrans_name;
      size_type nb_points;
      size_type firstreal;
      size_type firstface;
      size_type firsttype;
  };


  static const int NB_IM=4;

  static im_desc im_desc_tab[NB_IM] = {
    {"IM_NC(0,0)", "GT_PK(0,0)", 1, 0, 0, 0},
    {"IM_TRIANGLE(1)", "GT_PK(2,1)", 1, 1, 0, 1},
    {"IM_TRIANGLE(2)", "GT_PK(2,1)", 3, 4, 3, 2},
    {"IM_TRIANGLE(3)", "GT_PK(2,1)", 4, 13, 6, 5},
  };

  static const int NB_IMR=25; 

  static long_scalar_type im_desc_real[NB_IMR] = {
    // IM_NC(0,0)
  1.0,
    // IM_TRIANGLE(1)
  0.3333333333333333333333333333333333333333333333333333333333333333,
  0.3333333333333333333333333333333333333333333333333333333333333333,
  0.5,
    // IM_TRIANGLE(2)
  0.1666666666666666666666666666666666666666666666666666666666666666,
  0.1666666666666666666666666666666666666666666666666666666666666666,
  0.1666666666666666666666666666666666666666666666666666666666666666,
  0.6666666666666666666666666666666666666666666666666666666666666666,
  0.1666666666666666666666666666666666666666666666666666666666666666,
  0.1666666666666666666666666666666666666666666666666666666666666666,
  0.1666666666666666666666666666666666666666666666666666666666666666,
  0.6666666666666666666666666666666666666666666666666666666666666666,
  0.1666666666666666666666666666666666666666666666666666666666666666,
    // IM_TRIANGLE(3)
  0.3333333333333333333333333333333333333333333333333333333333333333,
  0.3333333333333333333333333333333333333333333333333333333333333333,
  -0.28125,
  0.2,
  0.2,
  0.2604166666666666666666666666666666666666666666666666666666666666,
  0.6,
  0.2,
  0.2604166666666666666666666666666666666666666666666666666666666666,
  0.2,
  0.6,
  0.2604166666666666666666666666666666666666666666666666666666666666,
  };

  static const int NB_IMF=9; 

  static const char * im_desc_face_meth[NB_IMF] = {
    // IM_TRIANGLE(1)
    "IM_GAUSS1D(1)","IM_GAUSS1D(1)","IM_GAUSS1D(1)",
    // IM_TRIANGLE(2)
    "IM_GAUSS1D(2)","IM_GAUSS1D(2)","IM_GAUSS1D(2)",
    // IM_TRIANGLE(3)
    "IM_GAUSS1D(3)","IM_GAUSS1D(3)","IM_GAUSS1D(3)",
  };

  static const int NB_IMN=9; 

  static size_type im_desc_node_type[NB_IMN] = {
    0,  // IM_NC(0,0)
    0,  // IM_TRIANGLE(1)
    0, 0, 0,  // IM_TRIANGLE(2)
    0, 0, 0, 0,  // IM_TRIANGLE(3)
  };

}

