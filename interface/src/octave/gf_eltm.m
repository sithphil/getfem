% FUNCTION [...] = gf_eltm([operation [, args]])
%
%   General constructor for eltm objects.
%
%   
%   This object represents a type of elementary matrix. In order to obtain a
%   numerical value of these matrices, see gf_mesh_im_get(mesh_im MI, 'eltm').
%   
%   If you have very particular assembling needs, or if you just want to check
%   the content of an elementary matrix, this function might be useful. But
%   the generic assembly abilities of gf_asm(...) should suit most needs.
%   
%
%   * E = gf_eltm('base', fem FEM)
%   return a descriptor for the integration of shape functions on
%   elements, using the fem `FEM`.
%
%   * E = gf_eltm('grad', fem FEM)
%   return a descriptor for the integration of the gradient of shape
%   functions on elements, using the fem `FEM`.
%
%   * E = gf_eltm('hessian', fem FEM)
%   return a descriptor for the integration of the hessian of shape
%   functions on elements, using the fem `FEM`.
%
%   * E = gf_eltm('normal')
%   return a descriptor for the unit normal of convex faces.
%
%   * E = gf_eltm('grad_geotrans')
%   return a descriptor to the gradient matrix of the geometric
%   transformation.
%
%   * E = gf_eltm('grad_geotrans_inv')
%   return a descriptor to the inverse of the gradient matrix of the
%   geometric transformation (this is rarely used).
%
%   * E = gf_eltm('product', eltm A, eltm B)
%   return a descriptor for the integration of the tensorial product of
%   elementary matrices `A` and `B`.
%
%
function [varargout]=gf_eltm(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_matlab('eltm', varargin{:});
  else
    gf_matlab('eltm', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
