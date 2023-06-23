  figure; 
t = 0:0.001:1;

%%
h = drawpolyline; 
axis([0 1 0 1]);


m=4;
for i=1:m
    c(i)=nchoosek(m-1,i-1);
end
% 构造系数矩阵
for i=1:1001
    for j=1:m
        A(i,j)=((1-t(1,i))^(m-j))*(t(1,i)^(j-1))*c(j);
    end
end
hold on;

hcurve = plot(bezier(h.Position,A), 'r', 'linewidth', 2);
axis([0 1 0 1]);
h.addlistener('MovingROI', @(h, evt) bezier(evt.CurrentPosition, A, hcurve));

clear;

%%
function p = bezier(p, A, h)
    
n=size(p,1)-1;
%求解曲线控制点
k=p*[1;1i];
D=zeros(3*n+1,3*n+1);
d=zeros(3*n+1,1);

%% 3n+1个方程
itr=1;
for i=0:n
    D(itr, 3 * i + 1)=1;
    d(itr) = k(i + 1);
    itr = itr + 1;
end
for i=1:n-1
    D(itr,3*i)=1;D(itr,3*i+1)=-2;D(itr,3*i+2)=1;
    itr=itr+1;
end
for i=1:n-1
    D(itr,3*i-1)=1;D(itr,3*i)=-2;D(itr,3*i+2)=2;D(itr,3*i+3)=-1;
    itr=itr+1;
end
D(itr,1)=1;D(itr,2)=-2;D(itr,3)=1;itr=itr+1;
D(itr,3*n-1)=1;D(itr,3*n)=-2;D(itr,3*n+1)=1;

%% 算出点
B=D\d;
q=zeros(1000*n+1,1);
for i=0:n-1
   q((1000*i+1):(1000*i+1001),:)=A*B(3*i+1:3*i+4,:);
end
p=q;

    if nargin>2,set(h, 'xdata', real(p), 'ydata', imag(p)); end
end


