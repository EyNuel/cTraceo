%% ==================================================================
%  
%  Traceo - Coherent Acoustic Pressure, Vertical Array
%
%  Written by Emanuel Ey	23/06/2011
%  Based on previous work by Orlando Rodriguez
%
%==================================================================

clear all%, close all 

imunit = sqrt( -1 );

case_title = '''Coherent Acoustic Pressure, Vertical Array''';

%% ==================================================================
%  
%  Define source data:
%  
%==================================================================

%disp('Defining source characteristics...')

freq   =  100;
Rmaxkm =  1.0; Rmax = Rmaxkm*1000;
Dmax   =  100; 

ray_step = Rmax/1000; 

zs = 25;
rs = 0;
thetamax = 30;
np2 = 101;
la = linspace(-thetamax,thetamax,np2);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [rs-1 Rmax];
source_data.f        = freq;
source_data.thetas   = la;

%% ==================================================================
%  
%  Define altimetry data:
%  
%==================================================================

%disp('Defining surface characteristics...')

altimetry = [rs-2 Rmax+2;0 0];

surface_data.type  =   '''V''';
surface_data.ptype =   '''H'''; % Homogeneous
surface_data.units =   '''W'''; % (Attenuation Units) dB/Wavelenght
surface_data.itype =  '''FL'''; % Sea surface interpolation type
surface_data.x     = altimetry; % Surface coordinates
surface_data.properties = [1510.0 300.0 2.0 0 0]; % [cp cs rho ap as]

%% ==================================================================
%  
%  Define sound speed data:
%  
%==================================================================

%disp('Defining the sound speed profile...')

c0 = 1500;

ssp_data.cdist = '''c(z,z)'''; % Sound speed profile

ssp_data.cclass = '''ISOV'''; % Isovelocity profile

ssp_data.z   = [0 Dmax]';
ssp_data.r   = [];
ssp_data.c   = [c0 c0]';

%% ==================================================================
%  
%  Define object data:
%  
%==================================================================

object_data.nobjects = 0; % Number of objects

%% ==================================================================
%  
%  Define bathymetry data:
%  
%==================================================================

%disp('Defining bottom characteristics...')

bathymetry = [rs-2 Rmax+2;Dmax/2 Dmax/2];

bottom_data.type   = '''R''' ; 
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  = '''W''' ; % Bottom attenuation units
bottom_data.itype  = '''FL'''; % Bottom interpolation type 
bottom_data.x      = bathymetry; % Bottom coordinates 
bottom_data.properties = [1550.0 0.0 2.0 0 0]; % [cp cs rho ap as]

%% ==================================================================
%  
%  Define output data:
%  
%==================================================================

%disp('Defining output options...')

output_data.array_shape = '''VRY''';
m = 1; %ranges = linspace(0,Rmax-1,m);
n = 16; depths = linspace(0,Dmax-1,n);

output_data.ctype       = '''CPR''';
output_data.r           = 900;
output_data.z           = depths;
output_data.miss        = 0.5;

%% ==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')

wtraceoinfil('flat.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

disp('Calling cTRACEO...')

!ctraceo flat 
disp('Reading the output data...')

%% Load and show coherent acoustic pressure:
load cpr 
p_c = p;
tl = -20.0*log10( abs(p.') ); 
tej = flipud( jet( 256 ) );
counter = 0; 

figure
imagesc(rarray,zarray,tl)
colorbar
hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

axis ij
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: Coherent TL from CPR (c)')
hold off
disp('done.')

%%
%{
disp('Calling fTRACEO...')

!traceo flat 

disp('Reading the output data...')


%% Load and show coherent acoustic pressure:
load cpr 
p_f = p(1,:) + imunit*p(2,:); 
tl = -20.0*log10( abs(p_f) ); 
tej = flipud( jet( 256 ) );
counter = 0; 

figure
imagesc(rarray,zarray,tl)
colorbar
hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

axis ij
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: Coherent TL from CPR (f)')
hold off
%}


