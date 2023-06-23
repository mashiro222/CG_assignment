%% optimize the algorithm by using the idea of inverse
function im2 = RBFImageWarp_optimized(im, psrc, pdst)


%% basic image manipulations
% get image (matrix) size
[h, w, dim] = size(im);
im2 = im;
imtemp = im;

%% initialization
n=size(psrc,1);
d=10000;
A=zeros(n,n);

%% exchange x and y coordinates(the reason will explain in the report)
ptemp=zeros(n,2);
ptemp(:,1)=psrc(:,2);ptemp(:,2)=psrc(:,1);psrc=ptemp;
ptemp(:,1)=pdst(:,2);ptemp(:,2)=pdst(:,1);pdst=ptemp;

%% assign value to A
% for i=1:1:n
%     for j=1:1:n
%         A(i,j)=f(i,psrc(j,1),psrc(j,2),d,psrc);% d=10000
%     end
% end
for i=1:1:n
    for j=1:1:n
        A(i,j)=f(i,pdst(j,1),pdst(j,2),d,pdst);% d=10000
    end
end

%% coefficent of the radial transportation
% a(:,1)=A'\(pdst(:,1)-psrc(:,1));
% a(:,2)=A'\(pdst(:,2)-psrc(:,2));
a(:,1)=A'\(psrc(:,1)-pdst(:,1));
a(:,2)=A'\(psrc(:,2)-pdst(:,2));

%% use loop to negate image
for i=1:h
    for j=1:w
        for k=1:dim 
            b=RBF(i,j,a,n,pdst,d);
            % find the origin point of the transportation and the four surrounding points
            x=b(1,1)+i;y=b(1,2)+j;
            x1=floor(x);y1=floor(y);
            x2=x1+1;y2=y1+1;
            % bilinear interpolation
            if x1>=1 && x2<=h && y1>=1 && y2<=w 
                temp=im2(x1,y1,k)*(x2-x)*(y2-y)+im2(x2,y1,k)*(x-x1)*(y2-y)+im2(x1,y2,k)*(x2-x)*(y-y1)+im2(x2,y2,k)*(x-x1)*(y-y1); 
                imtemp(i,j,k)=floor(temp);
            end
        end
    end
end

%% get the final result
im2=imtemp;

end




