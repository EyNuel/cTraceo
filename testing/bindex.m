function ind=bindex(xtest,binlb,clip_flag,method_flag)
% bindex: identifies which bin test points fall into
% usage: ind=bindex(xtest,binlb,clip_flag,method_flag)
%
% arguments:
%  xtest - set of one or more points to assign into a list of bins
%
%  binlb - list of lower bounds of bins in increasing order.
%          The last element in this vector is the upper bound
%          of the last bin.
%          Note: this vector must be sorted in increasing order.
%          I do not test for this property.
%
%  clip_flag - OPTIONAL scalar flag - binning has one problem -
%          i.e., what do you do with test points below the first
%          bin's lower bound or above the last bin?
%
%          clip_flag specifies whether test points below the
%          lowest bin are assigned bin 0 or bin 1. likewise
%          points above the highest bin must be dealt with too.
%          
%          nominally, a test point is in the i'th bin if
%          
%            binlb(i) <= xtest(j) < binlb(i+1)
%          
%          if a point falls below the lowest bin, then
%          this definition suggests it falls in bin 0.
%          above binlb(end), this definition puts a point
%          in bin number nbin.
%          
%          clip_flag==0  --> (the default) leave points outside
%                            the bins, and return 0 or nbins
%          clip_flag==1  --> put points below in bin# 1, and
%                            points above in bin# nbins-1
%
%  method_flag - OPTIONAL scalar flag - allows the user to
%          choose which binning algorithm to use. If not specified,
%          then the method is chosen via a simple heuristic.
%          
%          method_flag==1 --> loop over test points
%          method_flag==2 --> loop over bins
%          method_flag==3 --> sort
%          method_flag==4 --> hash table
%  
%  ind   - array of indicies of the bin(s) into which xtest fell


% author: John R. D'Errico
% email: woodchips@worldnet.att.net


% default for clip_flag
if (nargin<3)|isempty(clip_flag)
  clip_flag=0;
end


binlb=binlb(:);
nbins=length(binlb);
sizeind=size(xtest);
xtest=xtest(:);
ntest=length(xtest);


if (nbins==0)|(ntest==0)
  ind=[];
  return
end


% decide which method to use
if (nargin<4)|isempty(method_flag)
  r=nbins/ntest;
  if (r>20)&ntest<12
    method_flag=1;
  elseif (r<0.05)&nbins<12
    method_flag=2;
  elseif ntest>2500
    method_flag=4;
  else
    method_flag=3;
  end
end


switch method_flag
  case 1
    % case for small number of test points:
    ind=zeros(ntest,1);
    for i=1:ntest
      ind(i)=sum(xtest(i)>=binlb);
    end
  case 2
    % case for small number of bins
    ind=zeros(ntest,1);
    for i=1:nbins
      ind=ind+(xtest>=binlb(i));
    end
  case 3
    % sort solution
    [xxi,j] = sort(xtest);
    [dum,i] = sort([binlb;xxi]);
    ind(i) = 1:length(i);
    ind = (ind(nbins+(1:ntest)) - (1:ntest))';
    ind(j) = ind;
  case 4
    % hash table solution
    ind=zeros(ntest,1);
    mv=ind;
    
    b1=binlb(1);
    bn=binlb(end);
    
    % size of hash table is dependent on the number of nodes,
    % and how close to uniform the spacing is.
    ave_space=(bn-b1)/(nbins-1);
    min_space=min(diff(binlb));
    
    max_hash=1500;
    nhash=round(min(max_hash,(nbins-1)*ave_space/min_space));
    hash_x=b1+(bn-b1)*((1:nhash)'-.5)/(nhash);
    hash_table=[bindex(hash_x,binlb);nbins-1];
    
    ind(xtest>=bn)=nbins;
    k=find((xtest>=b1)&(xtest<bn));
    
    % estimate bin assignment
    ind(k)=hash_table(1+floor((xtest(k)-b1)/((bn-b1)/nhash)));
    
    % look for any points that are below their bin assignment.
    kdec=k(xtest(k)<binlb(ind(k)));
    while ~isempty(kdec)
      ind(kdec)=ind(kdec)-1;
      j=(xtest(kdec)>=binlb(ind(kdec)));
      if any(j)
        kdec(j)=[];
      end
    end
    
    % look for any points that are above their bin assignment
    kinc=k(xtest(k)>=binlb(ind(k)+1));
    while ~isempty(kinc)
      ind(kinc)=ind(kinc)+1;
      j=(xtest(kinc)<binlb(ind(kinc)+1));
      if any(j)
        kinc(j)=[];
      end
    end
    
end


% be nice and return indices as the same shape as xtest.
ind=reshape(ind,sizeind);


% do we return a bin index below 1 or above nbin-1?
if clip_flag
  ind=max(1,min(nbins-1,ind));
end









