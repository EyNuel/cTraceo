function wtraceoinfil( filename, thetitle, source_info, surface_info, ssp_info, object_info, bathymetry_info, output_info )

%**************************************************************************
% Writes Traceo input (waveguide) file. 
%
%**************************************************************************
% Revisions:
% 06.03.2012            Edited by Emanuel Ey,    SiPLab UAlg
%                       Created cTraceo.m which does the exact same thing
%                       as this function but without writing files to disk.
%                       NOTE: although it could be assumed that this would
%                       result in faster execution, it is in fact somewhat
%                       slower, so wtraceoinfil.m is still the recommended
%                       method.
%
% 05.03.2012 at 17:00   Edited by Emanuel Ey,    SiPLab UAlg
%                       Added support for irregularly spaced ray launching
%                       angles.
%
% 14/06/2011 at 11:30:  Edited by Emanuel Ey,    SiPLab UAlg
%                       Added this documentation as well some minor code
%                       cleanups.
%
% 27/07/2010 at 11:00:  Written by Tordar,       SiPLab UAlg
%
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
%       |---.thetas     :A vector containing the launching angles.
%       |
%       '---.dTheta     :When launching irregularly spaced rays, this
%                       parameter is required. It gives the step size
%                       between individual rays which for regularly spaced
%                       arrays is given by:
%                           dTheta = (theta(N) - theta(1)) / (nThetas - 1 )
%
%                       To retain compaitibility with previous versions of
%                       cTraceo examples, when dTheta is not provided the
%                       lowest spacing between any to adjacent launching 
%                       angles will used. This is obtained through:
%                           dTheta = min(abs(thetas(1:end-1)-thetas(2:end)))
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

separation_line(1:80) = '-';

%**************************************************************************
% Get source data: 

ds       = source_info.ds;
xs       = source_info.position;
rbox     = source_info.rbox;
freq     = source_info.f;
thetas   = source_info.thetas;
nThetas  = length( thetas );
if ~exist('source_info.dTheta')
    dTheta   = min(abs(thetas(1:end-1)-thetas(2:end)));
end

%**************************************************************************
% Get surface data: 

 atype = surface_info.type ;
aptype = surface_info.ptype; 
aitype = surface_info.itype;
  xati = surface_info.x    ;
  nati = length( xati(1,:) );
atiu   = surface_info.units;
aproperties = surface_info.properties;
  
%**************************************************************************
% Get sound speed data: 

cdist  = ssp_info.cdist;
cclass = ssp_info.cclass;

c   = ssp_info.c;
z   = ssp_info.z;
r   = ssp_info.r;

%**************************************************************************
% Get object data:

nobj  = object_info.nobjects ;

if nobj > 0  
 npobj = object_info.npobjects; 
 otype = object_info.type     ;
oitype = object_info.itype    ;
  xobj = object_info.x        ;
obju   = object_info.units    ;
oproperties = object_info.properties; 
end 

%**************************************************************************
% Get bathymetry data:

 btype = bathymetry_info.type ;
bptype = bathymetry_info.ptype; 
bitype = bathymetry_info.itype;
  xbty = bathymetry_info.x    ; nbty = length( xbty(1,:) );
btyu   = bathymetry_info.units;
bproperties = bathymetry_info.properties; 

%**************************************************************************
% Get output options: 
  
  calc_type = output_info.ctype      ;
array_shape = output_info.array_shape;
array_r     = output_info.r          ;
array_z     = output_info.z          ;
array_miss  = output_info.miss       ;

m = length( array_r ); 
n = length( array_z ); 

%**************************************************************************
% Write the WAVFIL: 
 
fid = fopen(filename,'w');
fprintf(fid,'%s\n',     thetitle);
fprintf(fid,'%s\n',     separation_line);
fprintf(fid,'%f\n',     ds);
fprintf(fid,'%f %f\n',  xs);
fprintf(fid,'%f %f\n',  rbox);
fprintf(fid,'%f\n',     freq);
fprintf(fid,'%d\n',     -nThetas);
fprintf(fid, '%f ', dTheta);
for i = 1:nThetas
    fprintf(fid, '%f ', thetas(i));
end
fprintf(fid, '\n');
fprintf(fid,'%s\n',     separation_line);
fprintf(fid,'%s\n',     atype);
fprintf(fid,'%s\n',     aptype);
fprintf(fid,'%s\n',     aitype);
fprintf(fid,'%s\n',     atiu);
fprintf(fid,'%d\n',     nati);
if aptype == '''H'''
fprintf(fid,'%f %f %f %f %f\n', aproperties);
fprintf(fid,'%e %f\n',          xati);
elseif aptype == '''N'''
   for i = 1:nati 
   fprintf(fid,'%f %f %f %f %f %f %f\n',[xati(i,1) xati(i,2) aproperties(i,1:5)]);
   end
else
   disp('Unknown surface properties...'), return   
end
fprintf(fid,'%s\n',separation_line); 
fprintf(fid,'%s\n',cdist);
fprintf(fid,'%s\n',cclass);
switch cdist
   case '''c(z,z)'''
	    nc = length( c );
	    zc = [z(:)';c(:)']; 
	    fprintf(fid,   '%d %d\n' ,[1 nc]);
	    fprintf(fid,'%f %f\n' ,zc);
   case '''c(r,z)'''
	    m = length( r ); 
	    n = length( z ); 
	    fprintf(fid,   '%d %d\n' ,[m n]);
	    fprintf(fid,'%e ',r);fprintf(fid,'\n');
	    fprintf(fid,'%e ',z);fprintf(fid,'\n');
        for ii = 1:n
            fprintf(fid,'%f ',c(ii,:)); fprintf(fid,'\n');
        end
   otherwise
            disp('Unknown sound speed distribution.'), return
end
fprintf(fid,'%s\n',separation_line);
fprintf(fid,'%d\n',nobj);
if nobj > 0
fprintf(fid,'%s\n',oitype);
for i = 1:nobj
    fprintf(fid,'%s\n',otype(i,:));
    fprintf(fid,'%s\n', obju(i,:));
    fprintf(fid,'%d\n',npobj(i));
    fprintf(fid,'%f %f %f %f %f\n', oproperties(i,:));
    for j = 1:npobj(i)
        fprintf(fid,'%f %f %f\n', xobj(i,1:3,j));
    end 
end
end
fprintf(fid,'%s\n',separation_line);
fprintf(fid,'%s\n', btype);
fprintf(fid,'%s\n',bptype);
fprintf(fid,'%s\n',bitype);
fprintf(fid,'%s\n', btyu);
fprintf(fid,'%d\n',nbty);
if bptype == '''H'''
	fprintf(fid,'%f %f %f %f %f\n',bproperties);
	fprintf(fid,'%e %f\n',xbty);
else
   for i = 1:nbty 
   fprintf(fid,'%f %f %f %f %f %f %f\n',[xbty(i,1) xbty(i,2) bproperties(i,1:5)]);
   end 
end
m = length( array_r );
n = length( array_z );
fprintf(fid,'%s\n',separation_line);
fprintf(fid,'%s\n',array_shape);
fprintf(fid,   '%d %d\n',m,n);
fprintf(fid,       '%e ',array_r);fprintf(fid,'\n');
fprintf(fid,       '%e ',array_z);fprintf(fid,'\n');
fprintf(fid,'%s\n',separation_line);
fprintf(fid,'%s\n',  calc_type);
fprintf(fid,'%f ',array_miss); fprintf(fid,'\n');
fclose( fid );
