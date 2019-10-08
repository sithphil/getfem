% FUNCTION [...] = gf_cvstruct_get(cvstruct CVS, [operation [, args]])
%
%   General function for querying information about convex_structure objects.
%   
%   The convex structures are internal structures of getfem++. They do not
%   contain points positions. These structures are recursive, since the faces
%   of a convex structures are convex structures.
%   
%
%   * n = gf_cvstruct_get(cvstruct CVS, 'nbpts')
%   Get the number of points of the convex structure.
%
%   * d = gf_cvstruct_get(cvstruct CVS, 'dim')
%   Get the dimension of the convex structure.
%
%   * cs = gf_cvstruct_get(cvstruct CVS, 'basic structure')
%   Get the simplest convex structure.
%   
%   For example, the 'basic structure' of the 6-node triangle, is the
%   canonical 3-noded triangle.
%
%   * cs = gf_cvstruct_get(cvstruct CVS, 'face', int F)
%   Return the convex structure of the face `F`.
%
%   * I = gf_cvstruct_get(cvstruct CVS, 'facepts', int F)
%   Return the list of point indices for the face `F`.
%
%   * s = gf_cvstruct_get(cvstruct CVS, 'char')
%   Output a string description of the cvstruct.
%
%   * gf_cvstruct_get(cvstruct CVS, 'display')
%   displays a short summary for a cvstruct object.
%
%
function [varargout]=gf_cvstruct_get(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_matlab('cvstruct_get', varargin{:});
  else
    gf_matlab('cvstruct_get', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
