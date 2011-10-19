% Matlab GetFEM++ interface
%
% Copyright (C) 2009-2011 Yves Renard.
%
% This file is a part of GetFEM++
%
% GetFEM++  is  free software;  you  can  redistribute  it  and/or modify it
% under  the  terms  of the  GNU  Lesser General Public License as published
% by  the  Free Software Foundation;  either version 2.1 of the License,  or
% (at your option) any later version.
% This program  is  distributed  in  the  hope  that it will be useful,  but
% WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
% or  FITNESS  FOR  A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.
% You  should  have received a copy of the GNU Lesser General Public License
% along  with  this program;  if not, write to the Free Software Foundation,
% Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA.
%
% Static equilibrium of an elastic solid in contact with a rigid foundation.
% Test of the different contact/friction formulations of Getfem.
%
% This program is used to check that matlab-getfem is working. This is also
% a good example of use of GetFEM++.
%

gf_workspace('clear all');
clear all;

% Import the mesh
m=gf_mesh('load', '../../../tests/meshes/disc_P2_h2.mesh');
% m=gf_mesh('load', '../../../tests/meshes/disc_P2_h1.mesh');
% m=gf_mesh('load', '../../../tests/meshes/disc_P2_h0.5.mesh');
% m=gf_mesh('load', '../../../tests/meshes/disc_P2_h0.25.mesh');
% m=gf_mesh('load', '../../../tests/meshes/disc_P2_h0.15.mesh');
d = gf_mesh_get(m, 'dim'); % Mesh dimension


% Parameters of the model
lambda = 1;  % Lame coefficient
mu = 1;      % Lame coefficient
friction_coeff = 0.4; % coefficient of friction
r = 100.0;      % Augmentation parameter
penalty_parameter = 1E-8;    % For rigid motions.
niter = 50;  % Maximum number of iterations for Newton's algorithm.
version = 9;  % 1 : frictionless contact and the basic contact brick
              % 2 : contact with 'static' Coulomb friction and basic contact brick
              % 3 : frictionless contact and the contact with a
              %     rigid obstacle brick
              % 4 : contact with 'static' Coulomb friction and the contact with a
              %     rigid obstacle brick
              % 5 : frictionless contact and the continuous brick
              %     Newton and Alart-Curnier augmented lagrangian,
              %     unsymmetric version
              % 6 : frictionless contact and the continuous brick
              %     Newton and Alart-Curnier augmented lagrangian, symmetric
              %     version.
              % 7 : frictionless contact and the continuous brick
              %     Newton and Alart-Curnier augmented lagrangian,
              %     unsymmetric version with an additional augmentation.
              % 8 : frictionless contact and the continuous brick
              %     New unsymmetric method.
              % 9 : frictionless contact and the continuous brick : Uzawa
              %     on the Lagrangian augmented by the penalization term.
              % 10 : contact with 'static' Coulomb friction and the continuous brick
              %     Newton and Alart-Curnier augmented lagrangian,
              %     unsymmetric version.
              % 11 : contact with 'static' Coulomb friction and the continuous brick
              %     Newton and Alart-Curnier augmented lagrangian,
              %     nearly symmetric version.
              % 12 : contact with 'static' Coulomb friction and the continuous brick
              %     Newton and Alart-Curnier augmented lagrangian,
              %     unsymmetric version with an additional augmentation.
              % 13 : contact with 'static' Coulomb friction and the continuous brick
              %     New unsymmetric method.
              % 14 : penalized frictionless contact (r is the penalization
              %     coefficient).
 % Signed distance representing the obstacle
if (d == 2) obstacle = 'y'; else obstacle = 'z'; end;

% Selection of the contact boundary
border = gf_mesh_get(m,'outer faces');
normals = gf_mesh_get(m, 'normal of faces', border);
contact_boundary=border(:, find(normals(d, :) < 0));
GAMMAC = 1;
gf_mesh_set(m, 'region', GAMMAC, contact_boundary);

% Plot the mesh
figure(1);
gf_plot_mesh(m, 'regions', [GAMMAC]);
title('Mesh and contact boundary (in red)');
pause(0.1);


% Finite element methods
mfu=gf_mesh_fem(m, d);
gf_mesh_fem_set(mfu, 'classical fem', 2);
mfd=gf_mesh_fem(m, 1);
gf_mesh_fem_set(mfd, 'classical fem', 2);
mflambda=gf_mesh_fem(m, 1); % used only by versions 5 to 13
gf_mesh_fem_set(mflambda, 'classical fem', 1);
mfvm=gf_mesh_fem(m, 1);
gf_mesh_fem_set(mfvm, 'classical discontinuous fem', 1);

% Integration method
mim=gf_mesh_im(m, 4);
mim_friction=gf_mesh_im(m, ...
    gf_integ('IM_STRUCTURED_COMPOSITE(IM_TRIANGLE(4),4)'));

% Volumic density of force
nbdofd = gf_mesh_fem_get(mfd, 'nbdof');
nbdofu = gf_mesh_fem_get(mfu, 'nbdof');
F = zeros(nbdofd*d, 1);
F(d:d:nbdofd*d) = -0.02;

% Elasticity model
md=gf_model('real');
gf_model_set(md, 'add fem variable', 'u', mfu);
gf_model_set(md, 'add initialized data', 'mu', [mu]);
gf_model_set(md, 'add initialized data', 'lambda', [lambda]);
gf_model_set(md, 'add isotropic linearized elasticity brick', mim, 'u', ...
                 'lambda', 'mu');
gf_model_set(md, 'add initialized fem data', 'volumicload', mfd, F);
gf_model_set(md, 'add source term brick', mim, 'u', 'volumicload');

% Small penalty term to avoid rigid motion (should be replaced by an
% explicit treatment of the rigid motion with a constraint matrix)
gf_model_set(md, 'add initialized data', 'penalty_param', ...
             [penalty_parameter]);          
gf_model_set(md, 'add mass brick', mim, 'u', 'penalty_param');

% The contact condition

cdof = gf_mesh_fem_get(mfu, 'dof on region', GAMMAC);
nbc = size(cdof, 2) / d;

solved = false;
if (version == 1 || version == 2) % defining the matrices BN and BT by hand
  contact_dof = cdof(d:d:nbc*d);
  contact_nodes = gf_mesh_fem_get(mfu, 'basic dof nodes', contact_dof);
  BN = sparse(nbc, nbdofu);
  for i = 1:nbc
    BN(i, contact_dof(i)) = -1.0;
    gap(i) = contact_nodes(d, i);
  end;
  if (version == 2)
    BT = sparse(nbc*(d-1), nbdofu);
    for i = 1:nbc
      BT(i*(d-1), contact_dof(i)-d+1) = 1.0;
      if (d > 2)
        BT(i*(d-1)+1, contact_dof(i)-d+2) = 1.0;
      end;
    end;
  end;

  gf_model_set(md, 'add variable', 'lambda_n', nbc);
  gf_model_set(md, 'add initialized data', 'r', [r]);
  if (version == 2)
    gf_model_set(md, 'add variable', 'lambda_t', nbc * (d-1));
    gf_model_set(md, 'add initialized data', 'friction_coeff', ...
                 [friction_coeff]);
  end;
  gf_model_set(md, 'add initialized data', 'gap', gap);
  gf_model_set(md, 'add initialized data', 'alpha', ones(nbc, 1));
  if (version == 1)
    gf_model_set(md, 'add basic contact brick', 'u', 'lambda_n', 'r', ...
        BN, 'gap', 'alpha', 0);
  else
    gf_model_set(md, 'add basic contact brick', 'u', 'lambda_n', ...
		 'lambda_t', 'r', BN, BT, 'friction_coeff', 'gap', 'alpha', 0);
  end;
elseif (version == 3 || version == 4) % BN and BT defined by contact brick

  gf_model_set(md, 'add variable', 'lambda_n', nbc);
  gf_model_set(md, 'add initialized data', 'r', [r]);
  if (version == 3)
    gf_model_set(md, 'add contact with rigid obstacle brick', mim, 'u', ...
	         'lambda_n', 'r', GAMMAC, obstacle, 0);
  else
    gf_model_set(md, 'add variable', 'lambda_t', nbc * (d-1));
    gf_model_set(md, 'add initialized data', 'friction_coeff', ...
		 [friction_coeff]);
    gf_model_set(md, 'add contact with rigid obstacle brick', mim, 'u', ...
	         'lambda_n', 'lambda_t', 'r', 'friction_coeff', GAMMAC, ...
		 obstacle, 0);
  end;

elseif (version >= 5 && version <= 8) % The continuous version, Newton
 
  ldof = gf_mesh_fem_get(mflambda, 'dof on region', GAMMAC);
  mflambda_partial = gf_mesh_fem('partial', mflambda, ldof);
  gf_model_set(md, 'add fem variable', 'lambda_n', mflambda_partial);
  gf_model_set(md, 'add initialized data', 'r', [r]);
  OBS = gf_mesh_fem_get(mfd, 'eval', { obstacle });
  gf_model_set(md, 'add initialized fem data', 'obstacle', mfd, OBS);
  gf_model_set(md, 'add continuous contact with rigid obstacle brick', ...
      mim_friction, 'u', 'lambda_n', 'obstacle', 'r', GAMMAC, version-4);
          
elseif (version == 9) % The continuous version, Uzawa
 
  ldof = gf_mesh_fem_get(mflambda, 'dof on region', GAMMAC);
  mflambda_partial = gf_mesh_fem('partial', mflambda, ldof);
  nbc = gf_mesh_fem_get(mflambda_partial, 'nbdof');
  lambda_n = zeros(1, nbc);
  % Not correct : the normal to the obstacle is assumed to be vertical.
  W=-gf_asm('boundary', GAMMAC, 'a=data(#2);V(#1)+=comp(vBase(#1).Base(#2))(:,2,i).a(i)', mim, mfu, mflambda_partial, lambda_n);
  indb=gf_model_set(md, 'add explicit rhs', 'u', W);
  OBS = gf_mesh_fem_get(mfd, 'eval', { obstacle });
  M = gf_asm('mass matrix', mim, mflambda_partial, mflambda_partial, GAMMAC);
  
  gf_model_set(md, 'add initialized data', 'r', [r]);
  OBS = gf_mesh_fem_get(mfd, 'eval', { obstacle });
  gf_model_set(md, 'add initialized fem data', 'obstacle', mfd, OBS);
  gf_model_set(md, 'add penalized contact with rigid obstacle brick', mim_friction, 'u', ...
	         'obstacle', 'r', GAMMAC);
  
  for ii=1:100
      disp(sprintf('iteration %d', ii));
      gf_model_get(md, 'solve', 'max_res', 1E-9, 'max_iter', niter); % , 'very noisy');
      U = gf_model_get(md, 'variable', 'u');
      lambda_n_old = lambda_n;
      % format long; lambda_n
      lambda_n = (M\ gf_asm('contact Uzawa projection', GAMMAC, mim_friction, mfu, U, mflambda_partial, lambda_n, mfd, OBS, r/10))';
      W=-gf_asm('boundary', GAMMAC, 'a=data(#2);V(#1)+=comp(vBase(#1).Base(#2))(:,2,i).a(i)', mim, mfu, mflambda_partial, lambda_n);
      gf_model_set(md, 'set private rhs', indb, W);
      difff = max(abs(lambda_n-lambda_n_old));
      disp(sprintf('diff : %g', difff/max(abs(lambda_n))));
      % pause;
      if (difff/max(abs(lambda_n)) < penalty_parameter) break; end;
  end;
  
  solved = true;
  
elseif (version >= 10 && version <= 13) % The continuous version with friction, Newton
 
  gf_mesh_fem_set(mflambda, 'qdim', 2);
  ldof = gf_mesh_fem_get(mflambda, 'dof on region', GAMMAC);
  mflambda_partial = gf_mesh_fem('partial', mflambda, ldof);
  gf_model_set(md, 'add fem variable', 'lambda', mflambda_partial);
  gf_model_set(md, 'add initialized data', 'r', [r]);
  gf_model_set(md, 'add initialized data', 'friction_coeff', [friction_coeff]);
  OBS = gf_mesh_fem_get(mfd, 'eval', { obstacle });
  gf_model_set(md, 'add initialized fem data', 'obstacle', mfd, OBS);
  gf_model_set(md, 'add continuous contact with friction with rigid obstacle brick', mim_friction, 'u', ...
	         'lambda', 'obstacle', 'r', 'friction_coeff', GAMMAC, version-9);
elseif (version == 14)
 
  % gf_model_set(md, 'add fem variable', 'lambda_n', mflambda_partial);
  gf_model_set(md, 'add initialized data', 'r', [r]);
  % gf_model_set(md, 'add initialized data', 'friction_coeff', [friction_coeff]);
  OBS = gf_mesh_fem_get(mfd, 'eval', { obstacle });
  gf_model_set(md, 'add initialized fem data', 'obstacle', mfd, OBS);
  gf_model_set(md, 'add penalized contact with rigid obstacle brick', mim_friction, 'u', ...
	         'obstacle', 'r', GAMMAC);
    
else
  error('Inexistent version');
end

% Solve the problem

if (~solved)
  gf_model_get(md, 'solve', 'max_res', 1E-9, 'very noisy', 'max_iter', niter,  'lsearch', 'simplest'); % , 'with pseudo potential');
end;

U = gf_model_get(md, 'variable', 'u');
% lambda_n = gf_model_get(md, 'variable', 'lambda_n');
VM = gf_model_get(md, 'compute_isotropic_linearized_Von_Mises_or_Tresca', ...
		  'u', 'lambda', 'mu', mfvm);
    

% set a custom colormap
% r=[0.7 .7 .7]; l = r(end,:); s=63; s1=20; s2=25; s3=48;s4=55; for i=1:s, c1 = max(min((i-s1)/(s2-s1),1),0);c2 = max(min((i-s3)/(s4-s3),1),0); r(end+1,:)=(1-c2)*((1-c1)*l + c1*[1 0 0]) + c2*[1 .8 .2]; end; colormap(r);

figure(2);
if (d == 3)
  gf_plot(mfvm, VM, 'mesh', 'off', 'cvlst', ...
          gf_mesh_get(mfdu,'outer faces'), 'deformation', U, ...
          'deformation_mf', mfu, 'deformation_scale', 1, 'refine', 8);
else
  gf_plot(mfvm, VM, 'deformed_mesh', 'on', 'deformation', U, ...
          'deformation_mf', mfu, 'deformation_scale', 1, 'refine', 8);
end;

title('Deformed configuration (not really a small deformation of course ...)');
colorbar;
pause(0.1);
