%==================================================================
%  
%  cTraceo - Ray Coordinates in a Pekeris flat waveguide with 2 objects
%  
%  Written by Tordar        Mexilhoeira Grande, Thu Dec 30 17:19:31 WET 2010
%  Revised by Emanuel Ey    04/07/2011
%  
%==================================================================

clear all%, close all 
case_title = '''Ray Coordinates in a Pekeris flat waveguide with 2 objects.''';

imunit = sqrt( -1 ); 

%==================================================================
%  
%  Define source data:
%  
%==================================================================

%disp('Defining source characteristics...')

freq   = 717;
Rmaxkm = 0.2; Rmax = Rmaxkm*1000;
Dmax   = 100; 

ray_step = Rmax/1000; 

zs = 25; rs = 0; thetamax = 85; np2 = 51; la = linspace(-thetamax,thetamax,np2);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [rs-1 Rmax+1];
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
surface_data.properties = [0 0 0 0 0]; % Dummy values

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

npo = 50; 

cobjp = 2000; cobjs = 200; rhoo = 5; alphao = 0;

R0 = 5; factor = 1.5; ro = [25 75]; zo = [75 25]; 

robj1 =  factor*R0*cos( linspace(pi,0,npo) ) + ro(1);
robj2 =  factor*R0*cos( linspace(pi,0,npo) ) + ro(2);
zup   =         R0*sin( linspace(pi,0,npo) );
zdn   =        -R0*sin( linspace(pi,0,npo) );

xobj1 = [robj1(:)';zdn(:)'+zo(1);zup(:)'+zo(1)]; 
xobj2 = [robj2(:)';zdn(:)'+zo(2);zup(:)'+zo(2)];

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

bathymetry = [rs-2 Rmax+2;Dmax Dmax];

bottom_data.type   = '''E''' ;
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  = '''W''' ; % Bottom attenuation units
bottom_data.itype  = '''FL'''; % Bottom interpolation type 
bottom_data.x      = bathymetry; % Bottom coordinates 
bottom_data.properties = [1700 0.0 1.7 0.7 0]; % [cp cs rho ap as]

%==================================================================
%  
%  Define output data:
%  
%==================================================================

%disp('Defining output options...')

output_data.ctype       = '''RCO'''; 
output_data.array_shape = '''RRY''';
output_data.r           = linspace(0, 100,201);
output_data.z           = linspace(0,Dmax,201);
output_data.miss        = 0.5;

%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')
wtraceoinfil('rco.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);

disp('Calling cTRACEO...')
!ctraceo rco

disp('Reading the output data...')
load rco 

figure, hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16) 
for i = 1:length(rays)
    plot(rays(i).r, rays(i).z)
end
fill(xobj1(1,:),xobj1(2,:),'k')
fill(xobj1(1,:),xobj1(3,:),'k')
fill(xobj2(1,:),xobj2(2,:),'k')
fill(xobj2(1,:),xobj2(3,:),'k')
grid on, box on
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTRACEO - Pekeris waveguide, ray coordinates')

disp('done.')
