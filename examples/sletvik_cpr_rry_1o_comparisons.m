%==================================================================
%  
%  cTraceo - Coherent Acoustic Pressure in Sletvik waveguide;
%            comparison of results with varying number of rays and raystep.
%
%  Written by Tordar        :Gambelas, Tue Dec  7 14:12:56 WET 2010
%  Revised by Emanuel Ey    :29/06/2011
%
%==================================================================
%==================================================================
%  
%  Traceo: Sletvik (Hopavagen) waveguide test.
%  Gambelas, Fri Feb 18 15:37:44 WET 2011
%  Written by Tordar 
%  
%==================================================================

addpath('../M-Files/');
addpath('../bin/');
clear all%, close all 
imunit = sqrt( -1 );

a = [100:150:550];  %raystep
b = [21:5:31];    %number of rays
numsubplots = length(a)*length(b);
spds = subplotdimensions(numsubplots);

minTL = [0];
maxTL = [0];

for i=1:length(a)
    for j=1:length(b)
        rayStep__= a(i);
        rayNumber__= b(j);
        subplotIndex = i + (j-1)*length(a);
        
        case_title = '''Coherent Acoustic Pressure in Sletvik waveguide; comparison of results with varying number of rays and raystep.''';

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

        ray_step = Rmax/rayStep__;

        zs = 3;
        rs = 0;

        thetamax = 30; np2 = rayNumber__; la = linspace(-thetamax,thetamax,np2);

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


        output_data.ctype       = '''CPR''';
        output_data.array_shape = '''RRY''';
        m = 101; ranges = linspace(0,Rmax-1,m);
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

        load cpr
        p = rp + imunit*ip;
        p_fTraceo = p;
        tl = 20.0*log10( abs(p) );

        %%
        figure
        pcolor(arrayR,arrayZ,tl), shading interp, %caxis([-20 -5]),
        colorbar
        hold on
        plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

        fill(robj,zup,'k')
        fill(robj,zdn,'k')
        plot(bathymetry(1,:),bathymetry(2,:),'k')
        grid on, box on
        axis([0 Rmax 0 Dmax])
        view(0,-90)
        hold off
        xlabel('Range (m)')
        ylabel('Depth (m)')
        title(sprintf('fTraceo - rays: %d; step: %d.',rayNumber__, rayStep__))
        %}
        disp('Calling cTraceo...')
        !ctraceo sletvik

        disp('Reading the output data...')
        load cpr

        p_cTraceo = p;
        tl = 20.0*log10( abs(p) );

        minTL = min( minTL, min(min(tl(isfinite(tl)))));
        maxTL = max( maxTL, max(max(tl(isfinite(tl)))));

        %%
        subplot(spds(1), spds(2), subplotIndex);
        figure(1)
        pcolor(arrayR,arrayZ,tl), shading interp, %caxis([-20 -5]),
%        colorbar
        hold on
        plot(rs,zs,'ko',rs,zs,'m*','MarkerSize',16)

        fill(robj,zup,'k')
        fill(robj,zdn,'k')
        plot(bathymetry(1,:),bathymetry(2,:),'k')
        grid on, box on
        axis([0 Rmax 0 Dmax])
        view(0,-90)
        hold off
        xlabel('Range (m)')
        ylabel('Depth (m)')
        title(sprintf('cTraceo - rays: %d; step: D/%d.',rayNumber__, rayStep__))        
    end
end

%subplot(spds(1), spds(2), :);
%caxis([minTL maxTL])
%colorbar

%% Compute differences:
%{
differ = (abs(p_fTraceo) - abs(p_cTraceo))./abs(p_fTraceo)*100;
differ(~isfinite(differ)) = 0;  %set inf and nan to zero
figure
pcolor(arrayR,arrayZ,differ), shading interp, caxis([min(min(differ)) max(max(differ))]),
colorbar
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTraceo vs fTraceo - relative amplitude difference (%).')

%differ = (angle(p_fTraceo) - angle(p_cTraceo))./angle(p_fTraceo)*100;
differ = (convang(angle(p_fTraceo), 'rad', 'deg') - convang(angle(p_cTraceo), 'rad', 'deg'))./convang(angle(p_fTraceo), 'rad', 'deg')*100;
differ(~isfinite(differ)) = 0;  %set inf and nan to zero
figure
pcolor(arrayR,arrayZ,differ), shading interp, caxis([min(min(differ)) max(max(differ))]),
colorbar
axis([0 Rmax 0 Dmax])
view(0,-90)
hold off 
xlabel('Range (m)')
ylabel('Depth (m)')
title('cTraceo vs fTraceo - relative phase difference (%).')

disp('done.')
%}
