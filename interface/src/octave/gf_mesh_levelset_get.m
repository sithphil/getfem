% FUNCTION [...] = gf_mesh_levelset_get(mesh_levelset MLS, [operation [, args]])
%
%   General function for querying information about mesh_levelset objects.
%   
%
%   * M = gf_mesh_levelset_get(mesh_levelset MLS, 'cut_mesh')
%   Return a mesh cut by the linked levelset's.
%
%   * LM = gf_mesh_levelset_get(mesh_levelset MLS, 'linked_mesh')
%   Return a reference to the linked mesh.
%
%   * nbls = gf_mesh_levelset_get(mesh_levelset MLS, 'nb_ls')
%   Return the number of linked levelset's.
%
%   * LS = gf_mesh_levelset_get(mesh_levelset MLS, 'levelsets')
%   Return a list of references to the linked levelset's.
%
%   * CVIDs = gf_mesh_levelset_get(mesh_levelset MLS, 'crack_tip_convexes')
%   Return the list of convex #id's of the linked mesh on
%   which have a tip of any linked levelset's.
%
%   * SIZE = gf_mesh_levelset_get(mesh_levelset MLS, 'memsize')
%   Return the amount of memory (in bytes) used by the mesh_levelset.
%
%   * s = gf_mesh_levelset_get(mesh_levelset MLS, 'char')
%   Output a (unique) string representation of the mesh_levelsetn.
%   
%   This can be used to perform comparisons between two
%   different mesh_levelset objects.
%   This function is to be completed.
%   
%
%   * gf_mesh_levelset_get(mesh_levelset MLS, 'display')
%   displays a short summary for a mesh_levelset object.
%
%
function [varargout]=gf_mesh_levelset_get(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_octave('mesh_levelset_get', varargin{:});
  else
    gf_octave('mesh_levelset_get', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;