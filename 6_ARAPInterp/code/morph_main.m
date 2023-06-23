[x,t] = readObj('elephant_s');
y = readObj('elephant_t');


%% set figure
amd = figure('position', [10 40 1210, 840]); subplot(131); drawmesh(t, x);
subplot(133); drawmesh(t, y); 
subplot(132); h=drawmesh(t, x);

%% 初始化
n = size(t, 1);
x=x(:,1:2);y=y(:,1:2);% z坐标就不要了
Q=zeros(2,2,n); 
X=zeros(2,2,n);% Q是平移矩阵，X是原像的差矩阵
Y=zeros(2,2,n);
theta=zeros(n,1);% 旋转角

%% 仿射变换拆解
for i=1:n
    X(:,:,i) = [x(t(i,1),:)-x(t(i,2),:);x(t(i,1),:)-x(t(i,3),:)];
    Y(:,:,i) = [y(t(i,1),:)-y(t(i,2),:);y(t(i,1),:)-y(t(i,3),:)];
    % 求解仿射变换系数矩阵
    A = (X(:,:,i)\ Y(:,:,i))';
    % 对A svd分解
    [U,sigma,V] = svd(A);
    Q(:,:,i) = V*sigma*V'; R=U*V';
    % 求出辅角并记录
    theta(i) = atan2(R(1,2), R(1,1));
end

%% bonus：解决旋转角一致性
theta=changetheta(theta,t,n);


%% 对系数矩阵赋值

% one = ones(n,1);
% M1=sparse(t(:,1),1:n,one);
% M2=sparse(t(:,2),1:n,-one);
% M3=sparse(t(:,1),(n+1):2*n,one);
% M4=sparse(t(:,3),(n+1):2*n,-one);
% M=M1+M2+M3+M4;
one=ones(size(theta));
jIndex = [t(:,1),t(:,2),t(:,1),t(:,3)];
iIndex = [1:n,1:n,n+(1:n),n+(1:n)];
vValue = [one;-one;one;-one];
M = sparse(iIndex,jIndex,vValue);




%% show result

% for t = 0:0.01:1
%     z = ARAP_interp(n, t, M, Q, theta, X);
%     set(h,'vertices',z);
%     drawnow; 
% end
% for t = 1:-0.01:0
%     z = ARAP_interp(n, t, M, Q, theta, X);
%     set(h,'vertices',z);
%     drawnow;
% end
    z = ARAP_interp(n, 0.5, M, Q, theta, X);
    set(h,'vertices',z);
    drawnow; 

%% function of show
function h = drawmesh(t, x)
    h = trimesh(t, x(:,1), x(:,2), x(:,1), 'facecolor', 'interp', 'edgecolor', 'k');
    axis equal; axis off; view(2); 
end
