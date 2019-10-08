% FUNCTION [...] = gf_spmat_set(spmat S, [operation [, args]])
%
%   Modification of the content of a getfem sparse matrix.
%   
%
%   * gf_spmat_set(spmat S, 'clear'[, list I[, list J]])
%   Erase the non-zero entries of the matrix.
%   
%   The optional arguments `I` and `J` may be specified to clear a
%   sub-matrix instead of the entire matrix.
%
%   * gf_spmat_set(spmat S, 'scale', scalar v)
%   Multiplies the matrix by a scalar value `v`.
%
%   * gf_spmat_set(spmat S, 'transpose')
%   Transpose the matrix.
%
%   * gf_spmat_set(spmat S, 'conjugate')
%   Conjugate each element of the matrix.
%
%   * gf_spmat_set(spmat S, 'transconj')
%   Transpose and conjugate the matrix.
%
%   * gf_spmat_set(spmat S, 'to_csc')
%   Convert the matrix to CSC storage.
%   
%   CSC storage is recommended for matrix-vector multiplications.
%
%   * gf_spmat_set(spmat S, 'to_wsc')
%   Convert the matrix to WSC storage.
%   
%   Read and write operation are quite fast with WSC storage.
%
%   * gf_spmat_set(spmat S, 'to_complex')
%   Store complex numbers.
%
%   * gf_spmat_set(spmat S, 'diag', mat D [, ivec E])
%   Change the diagonal (or sub-diagonals) of the matrix.
%   
%   If `E` is given, `D` might be a matrix and each column of `E` will
%   contain the sub-diagonal number that will be filled with the
%   corresponding column of `D`.
%
%   * gf_spmat_set(spmat S, 'assign', ivec I, ivec J, mat V)
%   Copy V into the sub-matrix 'M(I,J)'.
%   
%   `V` might be a sparse matrix or a full matrix.
%
%   * gf_spmat_set(spmat S, 'add', ivec I, ivec J, mat V)
%   Add `V` to the sub-matrix 'M(I,J)'.
%   
%   `V` might be a sparse matrix or a full matrix.
%
%
function [varargout]=gf_spmat_set(varargin)
  if (nargout),
    [varargout{1:nargout}]=gf_matlab('spmat_set', varargin{:});
  else
    gf_matlab('spmat_set', varargin{:});
    if (exist('ans', 'var') == 1), varargout{1}=ans; end;
  end;
% autogenerated mfile;
