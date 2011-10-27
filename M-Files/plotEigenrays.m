function plotEigenrays(eigenrays)
%Plots an eigenray structure as returned by cTraceo


[a, b] = size(eigenrays);   %get dimensions of hydrophone array

for rHyd = 1:a  %iterate over hydrophone ranges
    for zHyd = 1:b  %iterate over hydrophone depths
        for i = 1:eigenrays(rHyd,zHyd).nEigenrays   %iterate over eigenrays of hydrphone
            plot(eigenrays(rHyd,zHyd).eigenray(i).r, eigenrays(rHyd,zHyd).eigenray(i).z)   
        end
    end
end
