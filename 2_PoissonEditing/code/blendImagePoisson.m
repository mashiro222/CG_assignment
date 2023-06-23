function imret = blendImagePoisson(im1, im2, roi, targetPosition)
%matlab有自带函数，判断点在不在多边形内部；多边形内部的点与边界橡胶次数为奇数次，除了部分点，外部点就是偶数次

%% 为了符合正常规律，将im底数互换，然后roi的坐标不同也换了一下
temp=im1;im1=im2;im2=temp;
temp=roi(:,1);roi(:,1)=roi(:,2);roi(:,2)=temp;
temp=targetPosition(:,1);targetPosition(:,1)=targetPosition(:,2);targetPosition(:,2)=temp;

%% 对im1，im2进行处理，换成double型做算法
[h1, w1, dim1] = size(im1);
[h2, w2, dim2] = size(im2);
im=im2double(im1);
im1=im*255;
im=im2double(im2);
im2=im*255;

%% 计算拖动偏移
offset=[round(targetPosition(1,1)-roi(1,1)),round(targetPosition(1,2)-roi(1,2))];

%% 描述选中的原图和像图区域
Size=0;
btm=zeros(h1,w1);
rev=zeros(h1*w1,2);
in_im=zeros(h1,w1);
% size是方程未知数个数，btm为i，j对应序号,rev反过来
for i=1:h1
    for j=1:w1
        in_im(i,j)=inpolygon(i,j,roi(:,1),roi(:,2));
       if(in_im(i,j)==1)
           Size=Size+1;
           btm(i,j)=Size;% 表示坐标ij拉伸时到size位置
           rev(Size,1)=i;rev(Size,2)=j;% 表示拉伸之后size位置对应im1中的坐标
       end
    end
end

%% 构造方程组Ax=b
b=zeros(Size,1,dim1);
n=0;
for t=1:Size
    i=rev(t,1);j=rev(t,2);
    sum=in_im(i-1,j)+in_im(i+1,j)+in_im(i,j-1)+in_im(i,j+1);
    if sum==4
        n=n+1;rows(n)=t;cols(n)=btm(i,j);v(n)=-4;
        n=n+1;rows(n)=t;cols(n)=btm(i-1,j);v(n)=1;
        n=n+1;rows(n)=t;cols(n)=btm(i+1,j);v(n)=1;
        n=n+1;rows(n)=t;cols(n)=btm(i,j-1);v(n)=1;
        n=n+1;rows(n)=t;cols(n)=btm(i,j+1);v(n)=1;
        for k=1:dim1
            b(t,1,k)=im1(i+1,j,k)+im1(i-1,j,k)+im1(i,j-1,k)+im1(i,j+1,k)-4*im1(i,j,k);
        end
    else
        n=n+1;rows(n)=t;cols(n)=btm(i,j);v(n)=1;
        for k=1:dim1
            b(t,1,k)=im2(i+offset(1,1),j+offset(1,2),k);
        end
    end
end
A=sparse(rows,cols,v);

%% 求解方程组，采用lu分解优化
x=zeros(Size,1,dim1);
dA=decomposition(A,'lu');
for k=1:dim1
    x(:,:,k)=dA\b(:,:,k);
end

%% 将解赋值到像图对应区域
for t=1:Size
    for k=1:dim1
        i=rev(t,1);j=rev(t,2);
        im2(i+offset(1,1),j+offset(1,2),k)=x(t,1,k);
    end
end

%% compute blended image
imret = reshape(uint8(im2),[h2,w2,dim2]);
        
