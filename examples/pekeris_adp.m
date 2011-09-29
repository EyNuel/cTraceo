%==================================================================
%  
%  Traceo: Amplitudes and Arrival Times by Proximity in a Pekeris flat waveguide.
%  
%  Written by Tordar        Mexilhoeira Grande, Thu Dec 30 18:16:27 WET 2010
%  Revised by Emanuel Ey    01/07/2011
%  
%==================================================================

addpath('../M-Files/');
addpath('../bin/');
clear all%, close all 

case_title = '''Amplitudes and Arrival Times by Proximity in a Pekeris flat waveguide.''';
imunit = sqrt( -1 ); 

%==================================================================
%  
%  Define source data:
%  
%==================================================================

%disp('Defining source characteristics...')

freq   =  717;
Rmaxkm =    1; Rmax = Rmaxkm*1000;
Dmax   =  100; 

ray_step = Rmax/1000; 
zs       = 25;
rs       = 0;
thetamax = 30;
np2      = 2001;
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

ranges = Rmax; depths = 75; 

m = length( ranges );
n = length( depths );

output_data.ctype       = '''ADP'''; 
output_data.array_shape = '''RRY''';
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

disp('Calling cTraceo...')
!ctraceo flat

disp('Reading the output data...')
load aad

for i = 1:arrivals.nArrivals
	rh      = arrivals.rHyd;
	zh      = arrivals.zHyd;
	tau(i)  = arrivals.arrival(i).tau;
	a(i)    = arrivals.arrival(i).amp; 
end 
figure
stem(tau,abs(a)), box on, grid on
xlabel('Travel time (s)')
ylabel('Ray amplitude')
title('cTraceo - Amplitudes and Arrival Times by Proximity in a Pekeris flat waveguide.')


%{
disp('Calling fTRACEO...')
!traceo flat

disp('Reading the output data...')
load aad

for i = 1:nerays
	eval(['aad = aad' num2str(i) ';']); 
	rh = aad(1); 
	zh = aad(2); 
	tau(i) = aad(3); 
	a(i) = aad(4) + imunit*aad(5); 
	clear aad
end 
figure(1)
stem(tau,abs(a)), box on, grid on
xlabel('Travel time (s)')
ylabel('Ray amplitude')
title('fTRACEO - Amplitudes and Arrival Times by Proximity in a Pekeris flat waveguide.')
%}
disp('done.')
