%==================================================================
%  
%  cTraceo - Coherent Transmission Loss in Sletvik (Hopavagen) waveguide
%            with 1 object, along a linear array
%
%  Written by Tordar 		:Gambelas, Fri Feb 18 15:37:44 WET 2011
%  Revised by Emanuel Ey	:04/07/2011
%
%==================================================================

addpath('../M-Files/');
addpath('../bin/');
clear all%, close all 

imunit = sqrt( -1 );
minTL = 0;
maxTL = 0;

case_title = '''Coherent Transmission Loss in Sletvik (Hopavagen) waveguide with 1 object, along a linear array''';

object_ranges =  [5:1:110]; mo = length( object_ranges );
object_depths = [2:0.5:24]; no = length( object_depths );

%==================================================================
%  
%  Define source data:
%  
%==================================================================

%disp('Defining source characteristics...')

load sletvik_transect 

dr = ri(2) - ri(1);

freq   =  717;
Rmax   = max(ri); 
Dmax   =   27; 

ray_step = Rmax/1000; 

zs = 3; 
rs = 0;
 
thetamax = 30; np2 = 201; la = linspace(-thetamax,thetamax,np2);

source_data.ds       = ray_step;
source_data.position = [rs zs];
source_data.rbox     = [0-dr Rmax+dr];
source_data.f        = freq;
source_data.thetas   = la;

%==================================================================
%  
%  Define altimetry data:
%  
%==================================================================

%disp('Defining surface characteristics...')

load sletvik_transect 

altimetry = [min(ri)-2*dr max(ri)+2*dr;0 0];

surface_data.type  =   '''V'''; 
surface_data.ptype =   '''H'''; % Homogeneous
surface_data.units =   '''W'''; % (Attenuation Units) dB/Wavelenght
surface_data.itype =  '''FL'''; % Sea surface interpolation type
surface_data.x     = altimetry; % Surface coordinates
surface_data.properties = [0 0 0 0 0]; % Dummy values

%==================================================================
%  
%  Define sound speed data (ultra shallow sound speed profile): 
%  
%==================================================================

z = [0  10   15 20 25];
T = [12 12.5 12 8 6.5];
S = 32*ones( size( T ) );

c = mackenzie( z , T , S );

dz = 0.5; zc = [0:dz:25]; c = interp1(z,c,zc);
ssp_data.cdist = '''c(z,z)'''; % Sound speed profile
ssp_data.cclass = '''TABL'''; % Tabulated sound speed
ssp_data.z   = zc(:);
ssp_data.r   = [];
ssp_data.c   =  c(:);

%==================================================================
%  
%  Define object data:
%  
%==================================================================

object_data.nobjects = 1; % Number of objects

cobjp = 2000.0; cobjs = 200.0; rhoo = 5.0; alphao = 0.0;

npo = 50; R0 = 1; factor = 1;

[mo no i j];
ro = object_ranges(1); 
zo = object_depths(1);
[ro zo];
robj  =  factor*R0*cos( linspace(pi,0,npo) ) + ro;
zup   =         R0*sin( linspace(pi,0,npo) ) + zo;
zdn   =        -R0*sin( linspace(pi,0,npo) ) + zo;

xobj = [robj(:)';zdn(:)';zup(:)']; 

object_data.itype     = '''2P'''; % Objects interpolation type
object_data.npobjects =      npo; % Number of points in each object
object_data.type      = '''R'''; %
object_data.units     = '''W'''; % (Attenuation Units) Wavelenght 
object_data.properties = [cobjp cobjs rhoo alphao alphao];
object_data.x(1,1:3,:) = xobj;

%==================================================================
%  
%  Define bathymetry data:
%  
%==================================================================

%disp('Defining bottom characteristics...')

dr = ri(2) - ri(1); 

bathymetry(1,:) = [ri(1)-2*dr ri max(ri)+2*dr];
bathymetry(2,:) = [zi(1) zi' zi(end)]; 

bottom_data.type   = '''R''' ;
bottom_data.ptype  = '''H''' ; % Homogeneous bottom
bottom_data.units  = '''W''' ; % Bottom attenuation units
bottom_data.itype  = '''2P'''; % Bottom interpolation type 
bottom_data.x      = bathymetry; % Bottom coordinates 
bottom_data.properties = [1700 0.0 1.7 0.7 0]; % [cp cs rho ap as]

%==================================================================
%  
%  Define output data:
%  
%==================================================================

%disp('Defining output options...')

output_data.ctype       = '''CTL'''; 
output_data.array_shape = '''LRY''';
m = 201; ranges = linspace(0,Rmax-1,m);
n = 201; depths = linspace(0,Dmax-1,n);
output_data.r           = ranges;
output_data.z           = depths;
output_data.miss        = 0.5;


%==================================================================
%  
%  Call the function:
%  
%==================================================================

disp('Writing TRACEO waveguide input file...')
wtraceoinfil('sletvik.in',case_title,source_data,surface_data,ssp_data,object_data,bottom_data,output_data);


%%
%{
disp('Calling fTraceo...')
!traceo sletvik

disp('Reading the output data...')
load ctl 
        
minTL = min( minTL, min(min(tl(isfinite(tl)))));
maxTL = max( maxTL, max(max(tl(isfinite(tl)))));
        
%%
subplot(2,1,1)
imagesc(arrayR,arrayZ,tl)%, shading interp, %caxis([-20 -5]),
colorbar
hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

fill(robj,zup,'k')
fill(robj,zdn,'k')
plot(bathymetry(1,:),bathymetry(2,:),'k') 
grid on, box on
axis([0 Rmax 0 Dmax])
%view(0,-90)
axis ij
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('fTraceo - Sletvik waveguide, Coherent TL (from Acoustic Pressure) with 1 object.')
caxis([minTL maxTL])
colorbar
%}
%%
disp('Calling cTraceo...')
!ctraceo sletvik

disp('Reading the output data...')
load ctl

minTL = min( minTL, min(min(tl(isfinite(tl)))));
maxTL = max( maxTL, max(max(tl(isfinite(tl)))));


%subplot(2,1,2)
%draw the transmission loss
imagesc(arrayR,arrayZ,tl)%, shading interp, %caxis([-20 -5]),
colorbar
hold on

%draw the acoustic source
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

%draw the object
fill(robj,zup,'k')
fill(robj,zdn,'k')

%draw bathymetry
plot(bathymetry(1,:),bathymetry(2,:),'k') 

%draw hydrophone array
plot(ranges, depths, 's')

grid on, box on
axis([0 Rmax 0 Dmax])
axis ij
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTraceo - Sletvik waveguide, Coherent TL with 1 object.')
caxis([minTL maxTL])

%%
figure
plot(tl);
xlabel('Range (m)')
axis([0 Rmax 0 maxTL])
axis ij
