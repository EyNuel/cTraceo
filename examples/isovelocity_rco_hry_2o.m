%==================================================================
%  
%  cTraceo - Ray coordinates, Horizontal Array, 2 objects
%  
%  Written by Tordar, Gambelas, Tue Dec  7 14:12:56 WET 2010
%  Based on previous work by Orlando Rodriguez
%  
%==================================================================

clear all%, close all 

case_title = '''Ray coordinates, Horizontal Array, 2 objects''';
freq       =  100;
Rmaxboxkm  = 1.1; Rmaxbox = Rmaxboxkm*1000;
Rmaxhrykm  = 1.0; Rmaxhry = Rmaxhrykm*1000;
Dmax   =  100;
zs = 50; rs = 500; thetamax = 180; np2 = 22;
m = 21; ranges = linspace(0,Rmaxhry,m);
n =  1; zr     = 50;

%==================================================================
%  
%  Define source data:
%  
%==================================================================

ray_step = Rmaxbox/1000; 

la = linspace(-thetamax,thetamax,np2);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [-1 Rmaxbox];
source_data.f        = freq;
source_data.thetas   = la;

%==================================================================
%  
%  Define altimetry data:
%  
%==================================================================

%disp('Defining surface characteristics...')

altimetry = [-1 Rmaxbox+1;0 0];

surface_data.type  =   '''V'''; % 
surface_data.ptype =   '''H'''; % Homogeneous
surface_data.units =   '''W'''; % (Attenuation Units) Wavelenght
surface_data.itype =  '''FL'''; % Flat sea surface
surface_data.x     = altimetry; % Surface coordinates
surface_data.properties = [1510.0 600.0 0.99 0.1 0.0];

%==================================================================
%  
%  Define sound speed data:
%  
%==================================================================

%disp('Defining the sound speed profile...')

c0 = 1500;
ssp_data.cdist  = '''c(z,z)'''; % Sound speed profile
ssp_data.cclass = '''ISOV'''; % Isovelocity profile
ssp_data.z      = [0 Dmax]';
ssp_data.r      = [];
ssp_data.c      = [c0 c0]';

%==================================================================
%  
%  Define object data:
%  
%==================================================================

cobjp = 2000.0; cobjs = 200.0; rhoo = 5.0; alphao = 0.0;

npo = 50; 
R0 = 10; factor = 2; ro = [250 750]; zo = [40 60]; 

robj1 =  factor*R0*cos( linspace(pi,0,npo) ) + ro(1);
robj2 =  factor*R0*cos( linspace(pi,0,npo) ) + ro(2);
zup   =         R0*sin( linspace(pi,0,npo) ) + Dmax/2;
zdn   =        -R0*sin( linspace(pi,0,npo) ) + Dmax/2;

xobj1 = [robj1(:)';zdn(:)';zup(:)']; 
xobj2 = [robj2(:)';zdn(:)';zup(:)'];


object_data.nobjects =        2; % Number of objects
object_data.itype    = '''2P'''; % Objects interpolation type
object_data.npobjects(1) =  npo; % Number of points in each object 
object_data.npobjects(2) =  npo; % Number of points in each object
object_data.type(1,:)= '''R''' ; %
object_data.type(2,:)= '''R''' ; %
object_data.units(1,:) ='''W'''; % (Attenuation Units) Wavelenght 
object_data.units(2,:) ='''W'''; % (Attenuation Units) Wavelenght
object_data.properties(1,:) = [cobjp cobjs rhoo alphao alphao];
object_data.properties(2,:) = [cobjp cobjs rhoo alphao alphao];
object_data.x(1,1:3,:)      = xobj1;
object_data.x(2,1:3,:)      = xobj2;

%==================================================================
%  
%  Define bathymetry data:
%  
%==================================================================

%disp('Defining bottom characteristics...')

bathymetry = [-1 Rmaxbox+1;Dmax Dmax];

bottom_data.type   = '''R''' ;
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  =  '''W'''; % (Attenuation Units) Wavelenght
bottom_data.itype  = '''FL'''; % Bottom interpolation type 
bottom_data.x      = bathymetry;     % Bottom coordinates 
bottom_data.properties = [1550.0 600.0 2.0 0.1 0.0]; % Bottom properties (speed, speed, density, absorption coefficient)

%==================================================================
%  
%  Define output data:
%  
%==================================================================

%disp('Defining output options...')

depths = zr; 

output_data.ctype       = '''RCO'''; 
output_data.array_shape = '''HRY''';
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 0.5;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')
wtraceoinfil('flat.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

disp('Calling TRACEO...')
!ctraceo flat

disp('Reading the output data...')
load rco

figure(1), hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16) 
for i = 1:length(rays)
    plot(rays(i).r, rays(i).z)
end
fill(xobj1(1,:),xobj1(2,:),'k')
fill(xobj1(1,:),xobj1(3,:),'k')
fill(xobj2(1,:),xobj2(2,:),'k')
fill(xobj2(1,:),xobj2(3,:),'k')

plot(ranges, depths, 's')
 
grid on, box on
axis([0 Rmaxhry 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('TRACEO - Isovelocity waveguide, eigenrays')

%%
%{
disp('Writing TRACEO waveguide input file...')
wtraceoinfil('flat.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

disp('Calling TRACEO...')
!traceo flat

disp('Reading the output data...')

load rco

figure(1), hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16) 
for i = 1:length(thetas)
    eval(['ray = ray' num2str(i) ';']);
    r = ray(1,:);
    z = ray(2,:);
    plot(r,z)
    clear r z
end
fill(xobj1(1,:),xobj1(2,:),'k')
fill(xobj1(1,:),xobj1(3,:),'k')
fill(xobj2(1,:),xobj2(2,:),'k')
fill(xobj2(1,:),xobj2(3,:),'k')

plot(ranges, depths, 's')
 
grid on, box on
axis([0 Rmaxhry 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('TRACEO - Isovelocity waveguide, eigenrays')

disp('done.')
%}
