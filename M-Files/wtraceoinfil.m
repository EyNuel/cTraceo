function wtraceoinfil( filename, thetitle, source_info, surface_info, ssp_info, object_info, bathymetry_info, output_info )

% Writes Traceo input (waveguide) file. 
%
% SYNTAX: wtraceoinfil( filename, title, source, surface, ssp, object, bottom, output )
%

%*******************************************************************************
% Gambelas, 27/07/2010 at 11:00
% Written by Tordar 
%*******************************************************************************

separation_line(1:80) = '-';

%*******************************************************************************
% Get source data: 

ds       = source_info.ds;
xs       = source_info.position;
rbox     = source_info.rbox;
freq     = source_info.f;
thetas   = source_info.thetas; nthetas = length( thetas ); 
theta1   = thetas(1); thetan = thetas( nthetas );

%*******************************************************************************
% Get surface data: 

 atype = surface_info.type ;
aptype = surface_info.ptype; 
aitype = surface_info.itype;
  xati = surface_info.x    ; nati = length( xati(1,:) );
atiu   = surface_info.units;
aproperties = surface_info.properties;
  
%*******************************************************************************
% Get sound speed data: 

cdist  = ssp_info.cdist;
cclass = ssp_info.cclass;

c   = ssp_info.c;
z   = ssp_info.z;
r   = ssp_info.r;

%*******************************************************************************
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

%*******************************************************************************  
% Get bathymetry data:

 btype = bathymetry_info.type ;
bptype = bathymetry_info.ptype; 
bitype = bathymetry_info.itype;
  xbty = bathymetry_info.x    ; nbty = length( xbty(1,:) );
btyu   = bathymetry_info.units;
bproperties = bathymetry_info.properties; 

%*******************************************************************************  
% Get output options: 
  
  calc_type = output_info.ctype      ;
array_shape = output_info.array_shape;
array_r     = output_info.r          ;
array_z     = output_info.z          ;
array_miss  = output_info.miss       ;

m = length( array_r ); 
n = length( array_z ); 

%*******************************************************************************  
% Write the WAVFIL: 
 
fid = fopen(filename,'w');
fprintf(fid,'%s\n',thetitle);
fprintf(fid,'%s\n',separation_line);
fprintf(fid,'%f\n',ds);
fprintf(fid,'%f %f\n',xs);
fprintf(fid,'%f %f\n',rbox);
fprintf(fid,'%f\n',freq);
fprintf(fid,'%d\n',nthetas);
fprintf(fid,'%f %f\n',theta1,thetan);
fprintf(fid,'%s\n',separation_line);
fprintf(fid,'%s\n', atype);
fprintf(fid,'%s\n',aptype);
fprintf(fid,'%s\n',aitype);
fprintf(fid,'%s\n', atiu);
fprintf(fid,'%d\n',nati);
if aptype == '''H'''
fprintf(fid,'%f %f %f %f %f\n',aproperties);
fprintf(fid,'%e %f\n',xati);
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
