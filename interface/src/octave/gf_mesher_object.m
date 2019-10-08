% FUNCTION [...] = gf_mesher_object([operation [, args]])
%
%   General constructor for mesher_object objects.
%
%   This object represents a geometric object to be meshed by the
%   experimental meshing procedure of Getfem.
%   
%
%   * MF = gf_mesher_object('ball', vec center, scalar radius)
%   Represents a ball of corresponding center and radius.
%   
%
%   * MF = gf_mesher_object('half space', vec origin, vec normal_vector)
%   Represents an half space delimited by the plane which contains the
%   origin and normal to `normal_vector`. The selected part is the part
%   in the direction of the normal vector. This allows to cut a geometry
%   with a plane for instance to build a polygon or a polyhedron.
%   
%
%   * MF = gf_mesher_object('cylinder', vec origin, vec n, scalar length, scalar radius)
%   Represents a cylinder (in any dimension) of a certain radius whose axis
%   is determined by the origin, a vector `n` and a certain length.
%   
%
%   * MF = gf_mesher_object('cone', vec origin, vec n, scalar length, scalar half_angle)
%   Represents a cone (in any dimension) of a certain half-angle (in radians)
%   whose axis is determined by the origin, a vector `n` and a certain length.
%   
%
%   * MF = gf_mesher_object('torus', scalar R, scalar r)
%   Represents a torus in 3d of axis along the z axis with a great radius
%   equal to `R` and small radius equal to `r`. For the moment, the
%   possibility to change the axis is not given.
%   
%
%   * MF = gf_mesher_object('rectangle', vec rmin, vec rmax)
%   Represents a rectangle (or parallelepiped in 3D) parallel to the axes.
%   
%
%   * MF = gf_mesher_object('intersect', mesher_object object1 , mesher_object object2, ...)
%   Intersection of several objects.
%   
%
%   * MF = gf_mesher_object('union', mesher_object object1 , mesher_object object2, ...)
%   Union of several objects.
%   
%
%   * MF = gf_mesher_object('set minus', mesher_object object1 , mesher_object object2)
%   Geometric object being object1 minus object2.
%   
%
%
function [varargout]=gf_mesher_object(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_matlab('mesher_object', varargin{:});
  else
    gf_matlab('mesher_object', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
