function display(cs)
% gfSlice/display displays a short summary for a gfSlice object 
  for i=1:numel(cs),
    gf_slice_get(cs(i), 'display');
  end;
% autogenerated mfile;
