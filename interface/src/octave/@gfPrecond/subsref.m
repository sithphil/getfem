function [varargout]=subsref(obj, index)
% gfPrecond/subsref
  nout = max(nargout, 1); cnt=1;
  FGET = @gf_precond_get;
  FSET = @gf_precond_set;
  switch index(1).type
    case '{}'
      error('Cell array indexing not supported by gfPrecond objects')
    case '()'
      error('array indexing not supported by gfPrecond objects')
    case '.'
      switch index(1).subs
        case 'id'
          [varargout{1:nout}] = obj.id;
        case 'get'
          if (nargout) 
            h = FGET(obj, index(2).subs{:});
            if (isstruct(h) & isfield(h,'id') & isfield(h,'cid')), h = gfObject(h); end;
            [varargout{1:nargout}] = h;
          else
	     FGET(obj,index(2).subs{:});
            if (exist('ans', 'var') == 1)
              h=ans;
              if (isstruct(h) & isfield(h,'id') & isfield(h,'cid')), h = gfObject(h); end;
              varargout{1}=h;
            end;
          end;
          return;
        otherwise
          if ((numel(index) > 1) && (strcmp(index(2).type, '()')))
            h = FGET(obj,index(1).subs, index(2).subs{:});
            if (isstruct(h) & isfield(h,'id') & isfield(h,'cid')), h = gfObject(h); end;
            [varargout{1:nargout}] = h;
            cnt = cnt + 1;
          else
            h = FGET(obj, index(1).subs);
            if (isstruct(h) & isfield(h,'id') & isfield(h,'cid')), h = gfObject(h); end;
            [varargout{1:nargout}] = h;
          end
      end
  end
  % if there are others indexes, let matlab do its work
  if (numel(index) > cnt)
    for i=1:nout,
      varargout{i} = subsref(varargout{i}, index((cnt+1):end));
    end;
  end;
% autogenerated mfile;
