figure; 
t = 0:0.001:1;

%%
h = drawpolyline; 
axis([0 1 0 1]);

n=size(h.Position,1);
for i=1:n
    c(i)=nchoosek(n-1,i-1);
end
% 构造系数矩阵
for i=1:1001
    for j=1:n
        A(i,j)=((1-t(1,i))^(n-j))*(t(1,i)^(j-1))*c(j);
    end
end
hold on;

hcurve = plot(bezier(h.Position,A), 'g', 'linewidth', 2);
axis([0 1 0 1]);
h.addlistener('MovingROI', @(h, evt) bezier(evt.CurrentPosition, A, hcurve));

clear;

%%
function p = bezier(p, A, h)
    p = p*[1;1i];
    p=A*p;
    if nargin>2,set(h, 'xdata', real(p), 'ydata', imag(p)); end
end


