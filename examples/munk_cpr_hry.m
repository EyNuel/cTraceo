%==================================================================
% 
% cTraceo - Munk Profile, Deep Water, Coherent Acoustic Pressure along
%           a Horizontal Array
% 
% Written by Emanuel Ey,    30/06/2011
% Based on previous work by Orlando Rodriguez
%
%==================================================================

clear all%, close all 
disp('Deep water examples:') 
case_title = '''Munk Profile, Deep Water, Coherent Acoustic Pressure along a Horizontal Array''';

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
np2 = 51; thetamax = 14; la = linspace(-thetamax,thetamax,np2);

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

output_data.ctype       = '''CPR''';
output_data.array_shape = '''HRY''';
output_data.r           = linspace(0,100*1000,501);
output_data.z           = 800;
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
plot(rarray,tl)
%pcolor(rarray,zarray,tl), shading interp
axis([0 100*1000 60 120])
view(0,-90)
grid on, box on
xlabel('Range (m)')
ylabel('TL (dB)')
title('fTraceo.')
%}
%% --

disp('Calling cTRACEO...')
!ctraceo munk

disp('Reading the output data...')
load cpr

%calculating transmission loss from Acoustic Pressure.
tl = -20*log10( abs(p) ); 
size(tl)

figure
plot(rarray,tl)
axis([0 100*1000 60 120])
view(0,-90)
grid on, box on
xlabel('Range (m)')
ylabel('TL (dB)')
title('cTraceo.')

%{
load kraken_tlr.dat

kr  = 1000*kraken_tlr(:,1); 
ktl =      kraken_tlr(:,2); 

figure(1)
plot(rarray,tl,'--',kr,ktl)
axis([0 100*1000 60 120])
view(0,-90)
grid on, box on
xlabel('Range (m)')
ylabel('TL (dB)')
title('TRACEO vs. KRAKEN')
%}
disp('done.')
