%x = [0.1:0.1:0.3 1:3];
%y = sin( x );
%xi = linspace(0,3,200); 
x=[0:9];
%y=0.5*x.^3 +1.76*x.^2 -2*x +1;
y = sin(0.3*pi*x)
xi=[1.1:1.5:8];
figure(1);
plot(x,y);

'linear interpolation:'
yi = interp1(x,y,xi,'linear')

'Piecewise parabolic:'
[yi,yxi,yxxi] = ppinterp(x,y,xi)
'Cubic:'
%yi = interp1(x,y,xi,'cubic')
[yi,yxi,yxxi] = pcinterp(x,y,xi)
%{
figure(1)
plot(x,y,'o',xi,yi,'r-'), box on, grid on
xlabel('x')
ylabel('y(x)')
title('Piecewise Parabolic Interpolation test')
legend('exact','ppinterp')
%}
