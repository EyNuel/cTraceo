function drawWaveguide( thetitle, source_info, surface_info, ssp_info, object_info, bathymetry_info, output_info )

%**************************************************************************
% Plots a cTraceo input (waveguide). 
%
%**************************************************************************
% Revisions:
% 16/10/2012:  Written by Emanuel Ey,       SiPLab UAlg
%**************************************************************************
% INPUT STRUCTURES:
%   source_info         :A structure containing the source configuration
%       |
%       |---.ds         :The ray step [m].
%       |
%       |---.position   :A vector containing the source coodinates [m].
%       |               Format:
%       |                   [rx, zx]
%       |
%       |---.rbox       :A vector containing the "range box"; lower and 
%       |               upper boundaries for the ray range coordinates [m].
%       |               Format:
%       |                   [rbox1, rbox2]
%       |
%       |---.f          :Source frequency [Hz]
%       |
%       '---.thetas     :A vector containing the launching angles.
%                       The length of the vector, as well as the first 
%                       and the last elements will be used.
%        
%   surface_info        :A structure containing the surface (altimetry)
%       |               configuration.
%       |
%       |---.type       :A string defining the type of interface.
%       |               Allowed values are:
%       |                   '''A'''     Absorvent surface
%       |                   '''E'''     Elastic surface
%       |                   '''R'''     Rigid surface
%       |                   '''V'''     Vacuum beyond surface
%       |
%       |---.ptype      :A string defining the type of interface properties
%       |               Allowed values are:
%       |                   '''H'''     Homogeneous surface properties
%       |                   '''N'''     Non-Homogeneous surface properties
%       |
%       |---.properties :A vector if interface properties are homogeneous;
%       |               An array if properties are non-homogeneous, each line
%       |               representing the properties at a point of the surface.
%       |               Format:
%       |                   [cp,  cs,  rho,  ap,  as]
%       |                or:
%       |                   [cp1, cs1, rho1, ap1, as1;
%       |                    cp2, cs2, rho2, ap2, as2; ... ]
%       |               Where:
%       |                   cp  :Compressional speed                [m/s]
%       |                   cs  :Shear speed                        [m/s]
%       |                   rho :Density of medium beyond interface [kg/m3]
%       |                   ap  :Compressional attenuation   [see ".units"]
%       |                   as  :Shear attenuation           [see ".units"]
%       |
%       |---.units      :A string defining the units of the attenuation 
%       |               values given in the interface properties.
%       |               Allowed valueas are:
%       |                   '''F'''     dB/kHz
%       |                   '''M'''     dB/meter
%       |                   '''N'''     dB/neper
%       |                   '''Q'''     Q factor
%       |                   '''W'''     dB/<wavelength in meter>
%       |               
%       |---.x          :An array containing the surface coordinates.
%       |               Each line contains the range and depth coordinate
%       |               of a surface point.
%       |               Format:
%       |                   [r1, z1;
%       |                    r2, z2; ... ]
%       |
%       '---.itype      :A string defining the interpolation type used 
%                       for the surface.
%                       Allowed values are:
%                           '''FL'''    Flat surface
%                           '''SL'''    Surface with a Slope
%                           '''2P'''    Piecewise linear interpolation
%                           '''3P'''    Piecewise parabolic interpolation
%                           '''4P'''    Piecewise cubic interpolaton
%
%   ssp_info            :A structure containing the sound speed 
%       |               configuration.
%       |
%       |---.cdist      :A string defining the type of sound speed distribution.
%       |               Allowed values are:
%       |                    '''c(z,z)'''   Sound speed profile (range independent)
%       |                    '''c(r,z)'''   Sound speed field (range dependent)
%       |
%       |---.cclass     :A string defining what type of sound speed profile
%       |               to use. Allowed values are:
%       |                    '''ISOV'''     Isovelocity
%       |                    '''LINP'''     Linear
%       |                    '''PARP'''     Parabolic
%       |                    '''EXPP'''     Exponential
%       |                    '''N2LP'''     n2-linear
%       |                    '''ISQP'''     Inverse square gradient
%       |                    '''MUNK'''     Munk
%       |                    '''TABL'''     Tabulated
%       |
%       |---.r          :A vector containing the range coordinates.
%       |
%       |---.z          :A vector containing the depth coordinates.
%       |
%       '---.c          :A vector for sound speed profiles, or an array
%                       for sound speed fields.
%                           
%
%   object_info         :A structure containing optional object
%       |               configuration. 
%       |
%       |---.nobjects   :A scalar containinng the number of objects.
%       |
%       |---.itype      :A string determining what type of interpolation to
%       |               use for all objects. Allowed values are:
%       |                   '''2P'''    Piecewise Linear
%       |                   '''3P'''    Piecewise Parabolic 
%       |                   '''4P'''    Piecewise Cubic
%       |
%       |---.npobjects  :A vector, with each element containing the number
%       |               of points of an object.
%       |
%       |
%       |---.x          :A 3D array containing the coordinates of all
%       |               objects. Objects are composed of an upper a lower
%       |               surface, defined in pairs for a range coordinate.
%       |               Format: 
%       |                   x(<object_number>;
%       |                     <r>, <z_lower_surface>, <z_upper_surface>;
%       |                     <coord_index>)
%       |
%       |---.type       :Determines the object interface type.
%       |                   '''A'''     Absorvent surface
%       |                   '''E'''     Elastic surface
%       |                   '''R'''     Rigid surface
%       |                   '''V'''     Vacuum beyond surface
%       |
%       |---.units      :A vector of strings, with each string defining 
%       |               which attenuation untis to use for the
%       |               corresponding object properties.
%       |               Allowed valueas are:
%       |                   '''F'''     dB/kHz
%       |                   '''M'''     dB/meter
%       |                   '''N'''     dB/neper
%       |                   '''Q'''     Q factor
%       |                   '''W'''     dB/<wavelength in meter>
%       |
%       '---.properties :An Array, with each line representing the
%                       interface properties of the corresponding object.
%                       Format:
%                           [cp1, cs1, rho1, ap1, as1;
%                            cp2, cs2, rho2, ap2, as2; ... ]
%                       Where:
%                           cp  :Compressional speed                [m/s]
%                           cs  :Shear speed                        [m/s]
%                           rho :Density of medium beyond interface [kg/m3]
%                           ap  :Compressional attenuation   [see ".units"]
%                           as  :Shear attenuation           [see ".units"]
%
%   bathymetry_info     :Identical to surface_info (see above)
%
%   output_info         :A structure containing the desired output
%       |               configuration.
%       |
%       |---.ctype      :A string defining what calculation to perform.
%       |               Allowed values are:
%       |                   '''RCO'''   Ray Coordinates
%       |                   '''ARI'''   All Ray Information (RCO + ray amplitudes)
%       |                   '''ERF'''   Eigenrays, using Regula Falsi method
%       |                   '''EPR'''   Eigenrays, using Proximity
%       |                   '''ADR'''   Amplitudes and Delays, using Regula Falsi
%       |                   '''ADP'''   Amplitudes and Delays, using Proximity
%       |                   '''CPR'''   Coherent Acoustic Pressure
%       |                   '''CTL'''   Coherent Transmission Loss
%       |                   '''PVL'''   Coherent Particle Velocity
%       |                   '''PAV'''   Coherent Acoustic Pressure and Particle Velocity
%       |
%       |---.array_shape:A string defining the shape of the hydrophone array
%       |               Allowed values are:
%       |                   '''RRY'''   Rectangular Array
%       |                   '''HRY'''   Horizontal Array
%       |                   '''VRY'''   Vertical Array
%       |                   '''LRY'''   Linear Array
%       |
%       |---.r          :Range coordinates of hydrophone array
%       |
%       |---.z          :Depth coordinates of hydrophone array
%       |
%       '---.miss       :Determines distance threshhold for eigenray search.
%
%**************************************************************************



%figure, hold on
hold on

%% plot the sound speed profile (if existant)
%{
if strcmp(ssp_info.cdist, '''c(z,z)''') && strcmp(ssp_info.cclass, '''TABL''')
    subplot(1, 6, 1);
    plot(ssp_info.c, ssp_info.z);
    axis ij
    subplot(1, 6, [2 6])
    
else
    subplot(1,1,1);
end
%}

%% source

plot(source_info.position(1), source_info.position(2), 'ko');                   %draw a circle for the source
plot(source_info.position(1), source_info.position(2), 'm*','MarkerSize',16)    %draw a 'star' for the source



%% altimetry
plot(surface_info.x(1,:),surface_info.x(2,:),'k','LineWidth',1)

%% bathymetry
plot(bathymetry_info.x(1,:),bathymetry_info.x(2,:),'k','LineWidth',1)

%% others:
minR = min([surface_info.x(1,:), bathymetry_info.x(1,:), source_info.rbox(1)]);
maxR = max([surface_info.x(1,:), bathymetry_info.x(1,:), source_info.rbox(1)]);
minZ = min([surface_info.x(2,:), bathymetry_info.x(2,:)]);
maxZ = max([surface_info.x(2,:), bathymetry_info.x(2,:)]);

sizeR = maxR - minR;
sizeZ = maxZ - minZ;

%set plot area to be just bigger than waveguide
minR = minR -sizeR*0.03;
maxR = maxR +sizeR*0.03;
minZ = minZ -sizeZ*0.03;
maxZ = maxZ +sizeZ*0.03;

axis([minR, maxR, minZ, maxZ]);

view(0,-90)
xlabel('Range (m)')
ylabel('Depth (m)')

title(thetitle)
