%==================================================================
% 
% cTraceo - Deep Water, All Ray information with Munk Sound Speed
%           Field (range dependent).
% 
% Written by Tordar,        Gambelas, Thu Dec 23 17:57:39 WET 2010
% Revised by Emanuel Ey,    04/07/2011
% 
%==================================================================

clear all%, close all 
disp('Deep water examples:') 
case_title = '''Deep Water, All Ray information with Munk Sound Speed Field (range dependent).''';

%==================================================================
% 
% Define source data:
%
%==================================================================

%disp('Defining source characteristics...')

freq   =   100;
Rmaxkm =   101; Rmax = Rmaxkm*1000;
Dmax   =  5000; 

ray_step = Rmax/1000; 

zs = 1000; rs = 0;
np2 = 101; thetamax = 14; la = linspace(-thetamax,thetamax,np2);

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

np3 = 501;

altimetry(1,:) = linspace(rs-2,Rmax,np3);
altimetry(2,:) = 200*sin( (10*pi*altimetry(1,:)/Rmax) ).^2;

surface_data.type  =   '''V'''; % 
surface_data.ptype =   '''H'''; % Homogeneous
surface_data.units =   '''W'''; % (Attenuation Units) Wavelenght
surface_data.itype =  '''2P''';
surface_data.x     = altimetry; % Surface coordinates
surface_data.properties = [1510.0 600.0 0.99 0.1 0.0];

%==================================================================
%
% Define sound speed data:
%
%==================================================================

%disp('Defining the sound speed field...')

np = 51;

ranges = linspace(0,Rmax,np);
depths = linspace(0,Dmax,np);

rangeskm = ranges/1000; 

z1 = linspace(1000,2000,np); c1 = 1500;

for i = 1:np

    [c,cz,czz] = munk( depths , z1(i) , c1 );
    
    C(  :,i) = c(:);

end

ssp_data.cdist = '''c(r,z)'''; % Sound speed field
ssp_data.cclass = '''TABL''';
ssp_data.z    = depths(:);
ssp_data.r    = ranges(:);
ssp_data.c    = C;

%figure
%[c,h] = contour(ranges,depths,C,51); clabel(c,h)
%view(0,-90)
%xlabel('Range (m)')
%ylabel('Depth (m)')

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

bathymetry(1,:) = linspace(rs-2,Rmax,np3); aaux = ( bathymetry(1,:) - 0.5*Rmax );  
bathymetry(2,:) = -1500*exp(-aaux.^2/1e9) + Dmax;

bottom_data.type   = '''R''' ;
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  =  '''W'''; % (Attenuation Units) Wavelenght
bottom_data.itype  = '''2P'''; % Bottom interpolation type 
bottom_data.x      = bathymetry;     % Bottom coordinates 
bottom_data.properties = [1550.0 600.0 2.0 0.1 0.0]; % Bottom properties (speed, speed, density, absorption coefficient)

%==================================================================
%
% Define output data:
%
%==================================================================

%disp('Defining output options...')

ranges = Rmax; depths = Dmax; 

m = length( ranges );
n = length( depths );

output_data.ctype       = '''ARI''';
output_data.array_shape = '''RRY''';
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 0.5;

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
load ari 

nthetas = length( thetas ); 

figure, hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

for i=1:nthetas
    plot(rays(i).r, rays(i).z)
end

plot( altimetry(1,:), altimetry(2,:),'b')
plot(bathymetry(1,:),bathymetry(2,:),'k')
box on, grid on 
xlabel('Range (m)')
ylabel('Depth (m)')
title('TRACEO - Munk field, variable boundaries')
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off

disp('done.')
