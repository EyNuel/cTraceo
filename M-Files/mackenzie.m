function sound_velocity = mackenzie( Depths , Temperatures , Salinity ) ; 

%MACKENZIE: Converts Depth, Temperature and Salinity to Sound Velocity in Sea Water 
%through Mackenzie's formula. 
%
% Usage:    sound_speed_in_sea_water = mackenzie( depths , temperature , salinity ) ; 
%         
%           When applied to a vector 'mackenzie' returns a vector,  
%           when applied to a matrix 'mackenzie' returns a matrix 
%           after operating on every column. 
%           Range of validity: temperature 2 to 30 Celsius degrees, 
%           salinity 25 to 40 parts per thousand and depth 0 to 8000 m.  
%
%           See also chenmillero, coppens, delgrosso, leroy, medwin, 
%           depth2pressure and pressure2depth. 
 
%***************************************************************************************
% First version: 26/06/1998
% 
% Contact: orodrig@ualg.pt
% 
% Any suggestions to improve the performance of this 
% code will be greatly appreciated. 
% 
% Reference: K.V. Mackenzie, "Nine-term equation for the sound speed in the oceans",  
%            J. Acoust. Soc. Am. 70(3), pp 807-812, 1981.
%
%***************************************************************************************

coefficients_T  = [ 2.374e-4 -5.304e-2 4.591 1448.96 ] ; 
coefficients_D  = [ 1.675e-7  1.630e-2 0             ] ; 
coefficient_S   =   1.340     ;  
coefficient_TD  =  -7.139e-13 ; 
coefficient_TS  =  -1.025e-2  ;  

[ nrows ncolumns ] = size( Temperatures ) ; 

if min( [ nrows ncolumns ] ) == 1  
 
 temperatures = Temperatures  ; 
 depths       = Depths        ; 
 salinity     = Salinity - 35 ; 
 
 sound_velocity_T  = polyval( coefficients_T , temperatures ) ;  
 sound_velocity_D  = polyval( coefficients_D , depths       ) ;
 sound_velocity_S  = coefficient_S*salinity                   ; 
 sound_velocity_TD = coefficient_TD*( temperatures ).*( (depths).^3 ) ; 
 sound_velocity_TS = coefficient_TS*( temperatures.*salinity );   

 sound_velocity = sound_velocity_T + sound_velocity_D + sound_velocity_S + ... 
                  sound_velocity_TD +sound_velocity_TS ; 

else   

     for i = 1:ncolumns 

         temperatures = Temperatures( : , i )      ;
         depths       = Depths(       : , i )      ;
         salinity     = Salinity(     : , i ) - 35 ;

         sound_velocity_T  = polyval( coefficients_T , temperatures )  ;
         sound_velocity_D  = polyval( coefficients_D , depths       )  ;
         sound_velocity_S  = coefficient_S*salinity                    ;
         sound_velocity_TD = coefficient_TD*( temperatures ).*( (depths).^3 ) ;
         sound_velocity_TS = coefficient_TS*( temperatures.*salinity ) ;

         sound_velocity( : , i ) = sound_velocity_T  + sound_velocity_D + sound_velocity_S + ...           
                                   sound_velocity_TD + sound_velocity_TS ; 

     end % for i = 1:ncolumns 

end % if min( [ nrows ncolumns ] ) == 1 
