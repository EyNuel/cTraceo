%==================================================================
%  
%  cTraceo - Eigenray Search by Regula Falsi in a Pekeris flat waveguide.
%  
%  Written by Tordar        Faro, Thu Dec 23 23:56:16 WET 2010
%  Revised by Emanuel Ey    01/07/2011
%  
%==================================================================

clear all%, close all 
case_title = '''Eigenray Search by Regula Falsi in a Pekeris flat waveguide.''';

%==================================================================
%  
%  Define source data:
%  
%==================================================================

%disp('Defining source characteristics...')

freq   =  717;
Rmaxkm =    1.1; Rmax = Rmaxkm*1000;
Dmax   =  100; 

ray_step = Rmax/1000; 

zs       = 25;
rs       = 0;
thetamax = 30;
np2      = 501;
la       = linspace(-thetamax,thetamax,np2);

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

object_data.nobjects = 0; % Number of objects

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

ranges = 1000; depths = 75; 

m = length( ranges );
n = length( depths );

output_data.ctype       = '''ERF'''; 
output_data.array_shape = '''VRY''';
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

disp('Calling cTRACEO...')
!ctraceo flat

disp('Reading the output data...')
load eig

figure, hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16) 

[a, b] = size(eigenrays);   %get dimensions of hydrophone array

for rHyd = 1:a  %iterate over hydrophone ranges
    for zHyd = 1:b  %iterate over hydrophone depths
        for i = 1:eigenrays(a,b).nEigenrays   %iterate over eigenrays of hydrphone
            plot(eigenrays(a,b).eigenray(i).r, eigenrays(a,b).eigenray(i).z)   
        end
    end
end

plot(ranges,depths, 'm*');
grid on, box on
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTRACEO - Eigenray Search by Regula Falsi in a Pekeris flat waveguide.')

%%
%{
disp('Calling fTRACEO...')
!traceo flat

disp('Reading the output data...')
load eig

figure(1), hold on
plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16) 

for i = 1:nerays
%for i = 1:length(thetas)
eval(['ray = ray' num2str(i) ';']); 
r = ray(1,:); 
z = ray(2,:); 
plot(r,z)
clear r z 
end
plot(ranges,depths, 'm*');
grid on, box on
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('fTRACEO - Pekeris waveguide, eigenrays')

disp('done.')
%}

