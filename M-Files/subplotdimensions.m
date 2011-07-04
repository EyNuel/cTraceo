%spds = subplotdimensions(numsubplots);
function spds = subplotdimensions(numsubplots)

if nargin == 0
    help subplotdimensions %#ok<MCHLP>
%    pack

spds = [];


    return
end

spds(2) = ceil(sqrt(numsubplots));
spds(1) = floor(sqrt(numsubplots));
if prod(spds) <numsubplots
    spds(2) = spds(2)+1;
end
%  clear spds
clear numsubplots
return
