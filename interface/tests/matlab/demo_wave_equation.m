% Simple demo of a wave equation solved with transient bricks
% trace on;
gf_workspace('clear all');
m = gf_mesh('cartesian',[0:.2:1],[0:.2:1]);
% m=gf_mesh('import','structured','GT="GT_QK(2,1)";SIZES=[1,1];NOISED=1;NSUBDIV=[1,1];')

% create a mesh_fem of for a field of dimension 1 (i.e. a scalar field)
mf = gf_mesh_fem(m,1);
% assign the Q2 fem to all convexes of the mesh_fem,
gf_mesh_fem_set(mf,'fem',gf_fem('FEM_QK(2,2)'));

% Integration which will be used
mim = gf_mesh_im(m, gf_integ('IM_GAUSS_PARALLELEPIPED(2,4)'));

% detect the border of the mesh
border = gf_mesh_get(m,'outer faces');
% mark it as boundary #1
gf_mesh_set(m, 'boundary', 1, border);

% interpolate the initial data
U0 = gf_mesh_fem_get(mf, 'eval', { 'y.*(y-1).*x.*(x-1).*x.*x' });

md=gf_model('real');
gf_model_set(md, 'add fem variable', 'u', mf, 2);
transient_bricks = [gf_model_set(md, 'add Laplacian brick', mim, 'u')];
gf_model_set(md, 'add Dirichlet condition with multipliers', mim, 'u', mf, 1);

% transient part.
T = 15.0;
dt = 0.025;
theta = 0.5;
gf_model_set(md, 'add fem data', 'v', mf, 1, 2);
gf_model_set(md, 'add initialized data', 'dt', [dt]);
gf_model_set(md, 'add initialized data', 'theta', [theta]);
gf_model_set(md, 'add basic d2 on dt2 brick', mim, 'u', 'v', 'dt', 'theta');
gf_model_set(md, 'add theta method dispatcher', transient_bricks, 'theta');

% Initial data.
gf_model_set(md, 'variable', 'u',  U0, 2);
gf_model_set(md, 'first iter');

gf_model_get(md, 'listbricks');

% Iterations
for t = 0:dt:T
  gf_model_get(md, 'solve');
  gf_model_set(md, 'velocity update for order two theta method', 'u', 'v', 'dt', 'theta');
  U = gf_model_get(md, 'variable', 'u');
  V = gf_model_get(md, 'variable', 'v');

  subplot(2,1,1); gf_plot(mf, U, 'mesh', 'on', 'contour', .01:.01:.1); 
  colorbar; title(sprintf('computed solution u for t=%g', t));

  subplot(2,1,2); gf_plot(mf, V, 'mesh', 'on', 'contour', .01:.01:.1); 
  colorbar; title(sprintf('computed solution du/dt for t=%g', t));
  pause(0.1);

  gf_model_set(md, 'next iter');
end;






