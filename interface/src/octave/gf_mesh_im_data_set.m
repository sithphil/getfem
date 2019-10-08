% FUNCTION [...] = gf_mesh_im_data_set(mesh_im_data MID, [operation [, args]])
%
%   General function for modifying mesh_im objects
%   
%
%   * gf_mesh_im_data_set(mesh_im_data MID, 'region', int rnum)
%   Set the considered region to `rnum`.
%   
%
%   * gf_mesh_im_data_set(mesh_im_data MID, 'tensor size',)
%   Set the size of the data per integration point.
%   
%
%
function [varargout]=gf_mesh_im_data_set(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_matlab('mesh_im_data_set', varargin{:});
  else
    gf_matlab('mesh_im_data_set', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
