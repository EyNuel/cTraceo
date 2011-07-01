%==================================================================
% 
% cTraceo - Munk Profile, Deep Water, Eigenray Search by Regula Falsi
% 
% Written by Tordar,        Faro, Fri Dec 24 02:07:08 WET 2010
% Revised by Emanuel Ey,    30/06/2011
% 
%==================================================================

clear all%, close all 
disp('Deep water examples:') 
case_title = '''Munk Profile, Deep Water, Eigenray Search by Regula Falsi''';

%==================================================================
% 
% Define source data:
%
%==================================================================

%disp('Defining source characteristics...')

freq   =    50;
Rmaxkm =   101; Rmax = Rmaxkm*1000;
Dmax   =  5000; 

ray_step = Rmax/1000; 

zs = 1000; rs = 0;
np2 = 1001; thetamax = 14; la = linspace(-thetamax,thetamax,np2);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [rs-1 Rmax];
source_data.f        = freq;
source_data.thetas   = la;

%==================================================================
%
% Define altimetry data:
% 
%==================================================================

%disp('Defining surface characteristics...')


altimetry(1,:) = [rs-2 Rmax+2];
altimetry(2,:) = [0         0];

surface_data.type  =   '''V'''; % 
surface_data.ptype =   '''H'''; % Homogeneous
surface_data.units =   '''W'''; % (Attenuation Units) Wavelenght
surface_data.itype =  '''FL''';
surface_data.x     = altimetry; % Surface coordinates
surface_data.properties = [0 0 0 0 0.0]; % Dummy parameters

%==================================================================
%
% Define sound speed data:
%
%==================================================================

%disp('Defining the sound speed profile...')

c1 = 1500; z1 = 1300;

depths = linspace(0,Dmax,1001);

c = munk( depths, z1, c1 );
 
ssp_data.cdist = '''c(z,z)'''; % Sound speed profile
ssp_data.cclass = '''TABL''';
ssp_data.z    = depths(:);
ssp_data.r    = [];
ssp_data.c    = c(:);

%==================================================================
%  
%  Define object data:
%  
%==================================================================

object_data.nobjects = 0; % No objects

%==================================================================
% 
% Define bathymetry data:
%
%==================================================================

% Gaussian sea mountain: 

bathymetry(1,:) = [rs-2 Rmax+2];
bathymetry(2,:) = [Dmax   Dmax];

bottom_data.type   = '''E''' ;
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  =  '''W'''; % (Attenuation Units) Wavelenght
bottom_data.itype  = '''FL'''; % Bottom interpolation type 
bottom_data.x      = bathymetry;     % Bottom coordinates 
bottom_data.properties = [1550.0 600.0 2.0 0.1 0.0]; % Bottom properties (speed, speed, density, absorption coefficient)

%==================================================================
%
% Define output data:
%
%==================================================================

%disp('Defining output options...')

ranges = Rmax-100; depths = 1000; 

m = length( ranges );
n = length( depths );

output_data.ctype       = '''ERF''';
output_data.array_shape = '''RRY''';
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 10;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')
wtraceoinfil('munk.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

disp('Calling TRACEO...')
!ctraceo munk

disp('Reading the output data...')
load eig

nthetas = length( thetas ); 

figure(1), hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

[a, b] = size(eigenrays);   %get dimensions of hydrophone array

for rHyd = 1:a  %iterate over hydrophone ranges
    for zHyd = 1:b  %iterate over hydrophone depths
        for i = 1:eigenrays(a,b).nEigenrays   %iterate over eigenrays of hydrphone
            plot(eigenrays(a,b).eigenray(i).r,eigenrays(a,b).eigenray(i).z)   
        end
    end
end

plot(ranges,depths, 'm*');
plot( altimetry(1,:), altimetry(2,:),'b')
plot(bathymetry(1,:),bathymetry(2,:),'k')
box on, grid on 
xlabel('Range (m)')
ylabel('Depth (m)')
title('TRACEO - Munk profile, Eigenrays by Regula Falsi')
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off

disp('done.')
