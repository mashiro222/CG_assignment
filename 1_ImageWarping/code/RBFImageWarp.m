%% psrc=p source;pdst=p destiny
function im2 = RBFImageWarp(im, psrc, pdst)


%% basic image manipulations
% get image (matrix) size
[h, w, dim] = size(im);
im2 = im;
imtemp = im2;

%% initialization
n=size(psrc,1);
d=10000;
A=zeros(n,n);% A(i,j)=bi(pj)

%% exchange x and y coordinates(the reason will explain in the report)
ptemp=zeros(n,2);
ptemp(:,1)=psrc(:,2);ptemp(:,2)=psrc(:,1);psrc=ptemp;
ptemp(:,1)=pdst(:,2);ptemp(:,2)=pdst(:,1);pdst=ptemp;

%% assign value to A
for i=1:1:n
    for j=1:1:n
        A(i,j)=f(i,psrc(j,1),psrc(j,2),d,psrc);% d=10000
    end
end

%% coefficent of the radial transportation
a(:,1)=A'\(pdst(:,1)-psrc(:,1));
a(:,2)=A'\(pdst(:,2)-psrc(:,2));

%% use loop to negate image
for i=1:h
    for j=1:w
        for k=1:dim
            b=RBF(i,j,a,n,psrc,d);
            % transport only those points where the image point is still in range
            if round(b(1,1))+i<=h && round(b(1,1))+i>=1 && round(b(1,2))+j<=w && round(b(1,2))+j>=1 
                imtemp(round(b(1,1))+i,round(b(1,2))+j,k)=im2(i,j,k);
            end
        end
    end
end

%% get the final result
im2=imtemp;

end




