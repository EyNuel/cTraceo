%==================================================================
% 
% cTraceo - Munk Profile, Deep Water, Particle velocity  along
%           a Rectangular Array
% 
% Written by Emanuel Ey,    16/09/2011
% Based on previous work by Orlando Rodriguez
%
%==================================================================

addpath('../M-Files/');
addpath('../bin/');
clear all%, close all 

disp('Deep water examples:') 
case_title = '''Munk Profile, Deep Water, Particle velocity  along a Rectangular Array''';

imunit = sqrt( -1 ); 

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
bottom_data.units  = '''W''' ; % (Attenuation Units) dB/Wavelenght
bottom_data.itype  = '''FL'''; % Bottom interpolation type 
bottom_data.x      = bathymetry;     % Bottom coordinates 
bottom_data.properties = [1550.0 600.0 2.0 0.1 0.0]; % Bottom properties (speed, speed, density, absorption coefficient)

%==================================================================
%
% Define output data:
%
%==================================================================

%disp('Defining output options...')

output_data.ctype       = '''PVL''';
output_data.array_shape = '''RRY''';
output_data.r           = linspace(0,100*1000,300);
output_data.z           = linspace(0,5000,150);
output_data.miss        = 0.5;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')
wtraceoinfil('munk.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

%%
%{
disp('Calling fTRACEO...')
!traceo munk

disp('Reading the output data...')
load cpr

p_f = p; clear p 
p = p_f(1,:) + imunit*p_f(2,:);
tl = -20*log10( abs(p) ); 
size(tl)

figure
plot(arrayR,tl)
%pcolor(arrayR,arrayZ,tl), shading interp
axis([0 100*1000 60 120])
view(0,-90)
grid on, box on
xlabel('Range (m)')
ylabel('TL (dB)')
title('fTraceo.')
%}
%% --

disp('Calling cTraceo...')
!traceo munk

disp('Reading the output data...')
load pvl

%%
disp('Calculating horizontal component of particle velocity')
tlu = 20.0*log10( abs(u) ); 

minTL = 0;
maxTL = 0;
minTL = min( minTL, min(min(tlu(isfinite(tlu)))));
maxTL = max( maxTL, max(max(tlu(isfinite(tlu)))));

figure
%subplot(2,1,2)
pcolor(arrayR,arrayZ,tlu), shading interp, %caxis([-20 -5]),
colorbar
hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

%fill(robj,zup,'k')
%fill(robj,zdn,'k')
plot(bathymetry(1,:),bathymetry(2,:),'k') 
grid on, box on
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTraceo - Sletvik waveguide, horizontal component of particle velocity with 1 object.')
caxis([minTL maxTL])


disp('Calculating vertical component of particle velocity')
tlw = 20.0*log10( abs(w) ); 
figure
%subplot(2,1,2)
pcolor(arrayR,arrayZ,tlw), shading interp, %caxis([-20 -5]),
colorbar
hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

%fill(robj,zup,'k')
%fill(robj,zdn,'k')
plot(bathymetry(1,:),bathymetry(2,:),'k') 
grid on, box on
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTraceo - Sletvik waveguide, horizontal component of particle velocity with 1 object.')
caxis([minTL maxTL])
disp('done.')
