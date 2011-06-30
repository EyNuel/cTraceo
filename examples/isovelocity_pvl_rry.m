%==================================================================
%  
%  cTraceo - Particle Velocity, Rectangular Array
%
%  Written by Emanuel Ey, 30/06/2011
%  Based on previous work by Orlando Rodriguez
%
%==================================================================

clear all%, close all 
imunit = sqrt( -1 );
case_title = '''Particle Velocity, Rectangular Array''';

%==================================================================
%  
%  Define source data:
%  
%==================================================================
%disp('Defining source characteristics...')

freq   =  100;
Rmaxkm =  1.0; Rmax = Rmaxkm*1000;
Dmax   =  100; 

ray_step = Rmax/1000; 

zs = 25; rs = 0; thetamax = 30; np2 = 101; la = linspace(-thetamax,thetamax,np2);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [rs-1 Rmax];
source_data.f        = freq;
source_data.thetas   = la;

%==================================================================
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

bathymetry = [rs-2 Rmax+2;Dmax Dmax];

bottom_data.type   = '''R''' ; 
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  = '''W''' ; % Bottom attenuation units
bottom_data.itype  = '''FL'''; % Bottom interpolation type 
bottom_data.x      = bathymetry; % Bottom coordinates 
bottom_data.properties = [1550.0 0.0 2.0 0 0]; % [cp cs rho ap as]

%==================================================================
%  
%  Define output data:
%  
%==================================================================

%disp('Defining output options...')

output_data.array_shape = '''RRY''';
m = 7;
n = 5;
ranges = linspace(0,Rmax,m);
depths = linspace(0,Dmax,n); 

output_data.ctype       = '''PVL'''; 
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 1;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')

wtraceoinfil('flat.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

disp('Calling cTRACEO...')

!ctraceo flat

disp('Reading the output data...')

load pvl

tlu = 20.0*log10( abs(u) ); 
tlw = 20.0*log10( abs(w) );

figure
subplot(2,2,1)
imagesc(rarray(:,2:end-1),zarray,tlu(:,2:end-1));
hold on
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: TL u (c)')
colorbar
hold off

subplot(2,2,2)
imagesc(rarray(:,2:end-1),zarray,tlw(:,2:end-1));
hold on
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: TL w (c)')
colorbar
hold off

%%

disp('Calling fTRACEO...')
!traceo flat
disp('Reading the output data...')

load pvl

%p = rp + imunit*ip;
u = ru + imunit*iu;
w = rw + imunit*iw; 

%tl  = 20.0*log10( abs(p) ); 
tlu = 20.0*log10( abs(u) ); 
tlw = 20.0*log10( abs(w) );

%{
figure
pcolor(rarray,zarray,tl), shading interp, colorbar
hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)
view(0,-90)
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: TL')
hold off
%}

%figure
subplot(2,2,3)
%pcolor(rarray,zarray,tlu), shading interp, colorbar
imagesc(rarray(:,2:end-1),zarray,tlu(:,2:end-1));
hold on
%plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)
%view(0,-90)
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: TL u (f)')
colorbar
hold off

%figure
subplot(2,2,4)
%pcolor(rarray,zarray,tlw), shading interp, colorbar
imagesc(rarray(:,2:end-1),zarray,tlw(:,2:end-1));
hold on
%plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)
%view(0,-90)
xlabel('Range (m)')
ylabel('Depth (m)')
title('Isovelocity waveguide: TL w (f)')
colorbar
hold off

%{
figure
hold on
quiver(rarray(1:10:end),zarray(1:10:end), u(1:10:end, 1:10:end), w(1:10:end, 1:10:end),2)
%contour(rarray(1:1:end),zarray(1:1:end), tl(1:1:end, 1:1:end))
axis ij
hold off
disp('done.')
%}
