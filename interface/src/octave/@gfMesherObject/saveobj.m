function b=saveobj(a)
% gfMesherObject/saveobj
% this function is automatically called by matlab when objects of class
% gfMesherObjectare saved in a MAT-file
  b=a; b.txt=char(a);
% autogenerated mfile;
