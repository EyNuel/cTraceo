function [c,cz,czz] = munk( depths , z1 , c1 ) ;  

% Calculates Munk's deep water profile  
% Signal Processing Laboratory 
% 11/09/2006 at 11:00 
%
%SYNOPSIS:  [c,cz,czz] = munk( depths , z1 , c1 ) 
%
%           where z1 is the channel axis and c1 is the sound speed at z1 
%           (default values: z1 = 1200 , c1 = 1480); "cz" and "czz" are 
%           the corresponding sound speed first and second derivatives.    
%       
%           Example:  
%           depths = [ 0:10:4500 ]  ; z1 = 1400 ; c1 = 1450 ;
%           C0     = munk( depths ) ; 
%           C1     = munk( depths , z1 ) ;  
%           C2     = munk( depths , z1 , c1 ) ;     
%           figure(1), plot( C0 , depths , C1 , depths , '-.' , C2 , depths , '--' ) 
%           grid on, view( 0, -90 )  
%           xlabel( 'Sound Velocity (in m/s)' ), ylabel( 'Depth (in m)' )          

% written by TORDAR 

c = []; cz = c; czz = c; 

%Parameters taken from Buckingham92 

B  = 1.3e3 ; B2 = B*B;

epsilon = 7.37e-3 ;  

if nargin == 1 , z1 = 1200 ; c1 = 1480 ; end   

if nargin == 2 , c1 = 1480 ; end   

eta = 2*( depths - z1 )/B ; 

c   = 1 + epsilon*( eta + exp( -eta ) - 1 ); 
c   = c1*c ; 
cz  = ( 2*c1*epsilon/B  ) * ( 1 - exp( -eta ) );
czz = ( 4*c1*epsilon/B2 ) * exp( -eta ); 
