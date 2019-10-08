function [m,b]=gfSlice(a, varargin)
% gfSlice class pseudo-constructor
  this_class = 'gfSlice';
  if (nargin==0) error('cant create an empty slice reference'); end;
  if (isa(a,this_class)),
    m=a;
  else
    if (isstruct(a) & isfield(a,'id') & isfield(a,'cid'))
      cname = gf_workspace('class name',a);
      if (strcmp(cname, this_class))
	 m = a;
      else
	 m = gf_slice(a,varargin{:});
      end;
    else
      m = gf_slice(a,varargin{:});
    end;
    m.txt = '';
    m = class(m, this_class);
  end;
% autogenerated mfile;
