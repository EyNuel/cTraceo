function plotEigenrays(eigenrays)
%Plots an eigenray structure as returned by cTraceo


[a, b] = size(eigenrays);   %get dimensions of hydrophone array

for rHyd = 1:a  %iterate over hydrophone ranges
    for zHyd = 1:b  %iterate over hydrophone depths
        for i = 1:eigenrays(a,b).nEigenrays   %iterate over eigenrays of hydrphone
            plot(eigenrays(a,b).eigenray(i).r, eigenrays(a,b).eigenray(i).z)   
        end
    end
end
