%==================================================================
%  
%  cTraceo: Pekeris waveguide with block example.
%  
%  Written by Tordar        Gambelas, Tue Dec  7 15:29:34 WET 2010
%  Revised by Emanuel Ey    29/06/2011
%  
%==================================================================

clear all, close all 

imunit = sqrt( -1 ); 

case_title = '''Pekeris waveguide with block example''';
freq       =  100;
Rmaxboxkm  = 5.1; Rmaxbox = Rmaxboxkm*1000;
Rmaxhrykm  = 5.0; Rmaxhry = Rmaxhrykm*1000;
Dmax   =  1000;
zs = 500; rs = 0; thetamax = 20; nthetas = 301;
m = 501; n =  251;
ranges = linspace(-Rmaxhry,Rmaxhry,m);
zr     = linspace(0,Dmax,n);

%==================================================================
%  
%  Define source data:
%  
%==================================================================

ray_step = Rmaxbox/1000; 

la = linspace(-thetamax,thetamax,nthetas);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [-Rmaxbox Rmaxbox];
source_data.f        = freq;
source_data.thetas   = la;

%==================================================================
%  
%  Define altimetry data:
%  
%==================================================================

%disp('Defining surface characteristics...')

altimetry = [-Rmaxbox-1 Rmaxbox+1;0 0];

surface_data.type  =   '''V''';
surface_data.ptype =   '''H'''; % Homogeneous
surface_data.units =   '''W'''; % (Attenuation Units) dB/Wavelenght
surface_data.itype =  '''FL'''; % Sea surface interpolation type
surface_data.x     = altimetry; % Surface coordinates
surface_data.properties = [0 0 0 0 0]; % [cp cs rho ap as]

%==================================================================
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

%==================================================================
%  
%  Define object data:
%  
%==================================================================

object_data.nobjects = 0; % Number of objects

%==================================================================
%  
%  Define bathymetry data:
%  
%==================================================================

%disp('Defining bottom characteristics...')

bathymetry = [-Rmaxbox-1 2000 2010 2990 3000 Rmaxbox+1;...
               Dmax      Dmax  500  500 Dmax Dmax];

bottom_data.type   = '''E''' ; 
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  = '''W''' ; % Bottom attenuation units
bottom_data.itype  = '''2P'''; % Bottom interpolation type 
bottom_data.x      = bathymetry; % Bottom coordinates 
bottom_data.properties = [2000.0 0 2 0.5 0]; % [cp cs rho ap as]

%==================================================================
%  
%  Define output data:
%  
%==================================================================

%disp('Defining output options...')

output_data.array_shape = '''RRY''';

depths = zr; 

output_data.ctype       = '''CPR'''; 
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 0.5;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')
wtraceoinfil('block_cpr_rry.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);
%{
disp('Calling fTRACEO...')
!traceo block_cpr_rry

disp('Reading and displaying the result...')
load cpr

p = rp + imunit*ip;
tl = -20*log10( abs( p ) ); 
%}

disp('Calling cTRACEO...')
!ctraceo block_cpr_rry

disp('Reading and displaying the result...')
load cpr

tl = -20*log10( abs( p ) ); 

tej = flipud( jet( 256 ) );

figure, hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16) 
pcolor(rarray,zarray,-20*log10( abs(p) )), shading interp, colormap( tej ), caxis([30 80]), colorbar
plot(bathymetry(1,:),bathymetry(2,:),'k','LineWidth',1)
axis([-Rmaxhry Rmaxhry 0 Dmax])
hold off
view(0,-90)
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTRACEO - Block case')

disp('done.')
