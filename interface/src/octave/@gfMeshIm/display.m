function display(cs)
% gfMeshIm/display displays a short summary for a gfMeshIm object 
  for i=1:numel(cs),
    gf_mesh_im_get(cs(i), 'display');
  end;
% autogenerated mfile;