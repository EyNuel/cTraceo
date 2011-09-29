%==================================================================
%  
%  cTraceo - All Ray Information.
%
%  Written by Tordar 		:Faro, Sat Dec 18 17:39:49 WET 2010
%  Revised by Emanuel Ey	:29/06/2011
%
%==================================================================

addpath('../M-Files/');
addpath('../bin/');
clear all%, close all 

imunit = sqrt( -1 );

case_title = '''All Ray Information''';

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

zs = 25; rs = 0; thetamax = 30; np2 = 301; la = linspace(-thetamax,thetamax,np2);

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
m = 51; n = 21; ranges = linspace(0,Rmax,m); depths = linspace(0,Dmax,n); 

output_data.ctype       = '''ARI'''; 
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 1;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')

wtraceoinfil('flat_ari.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);


disp('Calling cTraceo...')


!ctraceo flat_ari

disp('Reading the output data...')

load ari

nthetas = size(rays,1); 

figure, hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)
for i = 1:nthetas
   plot(rays(i).r,    rays(i).z)
end 
plot( altimetry(1,:), altimetry(2,:),'b')
plot(bathymetry(1,:),bathymetry(2,:),'k')
box on, grid on 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTraceo - Munk profile, variable boundaries')
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off

%rays(1).r(151:155)


%%
%{
disp('Calling fTRACEO...')

!traceo flat_ari

disp('Reading the output data...')

load ari

nthetas = length( thetas ); 

figure, hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)
for i = 1:nthetas
   rayname = ['ray' num2str(i) ];
   rayexist = exist( rayname ); 
   if rayexist == 1 
     eval(['ray = ray' num2str(i) ';'])
     r = ray(1,:);
     z = ray(2,:);
     tau = ray(3,:);
     amp = abs( ray(4,:)+ j*ray(5,:));
     eval(['amp_f' num2str(i) ' = ray(4,:)+ j*ray(5,:);']);
%     cline(r, z,tau)
     plot(r,z)
   end 
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
%}
%amp_diff = zeros(nthetas);

%{
for i = 1:nthetas
    eval(['amp_diff' num2str(i) ' = amp_c' num2str(i) '(1,:) - amp_f' num2str(i) '(1,:);'])
    size(find(amp_diff > 1e-9))
end
%}
