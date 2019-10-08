% FUNCTION [...] = gf_mesh_im_data([operation [, args]])
%
%   General constructor for mesh_im_data objects.
%
%   This object represents data defined on a mesh_im object.
%   
%
%   * MIMD = gf_mesh_im_data(mesh_im mim, int region, ivec size)
%   Build a new mesh_imd object linked to a mesh_im object. If `region` is
%   provided, considered integration points are filtered in this region.
%   `size` is a vector of integers that specifies the dimensions of the
%   stored data per integration point. If not given, the scalar stored
%   data are considered.
%   
%
%
function [varargout]=gf_mesh_im_data(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_matlab('mesh_im_data', varargin{:});
  else
    gf_matlab('mesh_im_data', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
