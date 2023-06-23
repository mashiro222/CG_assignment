
clear all
[x, t] = readObj('Balls.obj');


B = findBoundary(x, t);
nv = size(x, 1);
I = setdiff(1:nv, B);
nf = size(t, 1);
AdjMat=sparse(t, t(:, [2 3 1]), repmat((1:nf)',3,1), nv, nv);

Lt = sparse(nv, nv);    
arr=zeros(nv,1);
for i=I
    ti = nonzeros(AdjMat(:,i));
    c=t(ti,:);
    [ci, cj] = find(c==i);
    c =  c( sub2ind(size(c), [ci ci], mod([cj cj+1], 3)+1) );
    k=size(c,1);
    arr(c(:,1))=c(:,2);
    ring=c(1,1)*ones(k,1);
    for j=2:k
        ring(j)=arr(ring(j-1));
    end
    n = numel(ring);
    lena = vecnorm( x(i, :)-x(ring,:), 2, 2 );
    lenb = vecnorm( x(ring, :)-x(ring([2:end 1]),:), 2, 2 );

    angles = acos( (lena.^2 + lena([2:end 1]).^2 - lenb.^2)./(2*lena.*lena([2:end 1])) );
    angles = [0; angles(1:end-1)]/sum(angles) * 2*pi;
    a = cumsum(angles);
    y = lena.*exp(1i*a);
   
    tt = zeros(n, 3);
    for j=1:n
        k = find(a>pi, 1, 'first');
        tt(j, :) = j+[0 k-2 k-1];

        a = [0; a(3:end)-a(2); 2*pi-a(2)];
    end
    tt = mod(tt-1, n)+1;

    fAreas = @(i, j) real(y(i)).*imag(y(j)) - imag(y(i)).*real(y(j));
    
    wt = [fAreas(tt(:,2), tt(:,3)) fAreas(tt(:,3), tt(:,1)) fAreas(tt(:,1), tt(:,2))];
    Lt(:, i) = sparse(ring(tt), 1, wt./sum(wt, 2), nv, 1)/n;

end

L = Lt';
L = spdiags(-sum(L,2), 0, L);

y = zeros(nv, 1);
y(B) = exp(2i*pi*(1:numel(B))/numel(B));
y(I) = -L(I,I)\L(I,B)*y(B);

figure; drawmesh(t, y);

z(:,1)=real(y);z(:,2)=imag(y);
for i=1:size(z,1)
    for j=1:2
        fprintf("%f, ",z(i,j));
    end
    fprintf("\n");
end



% function catheadplanar = Tutte_floater(cathead)
% 
% %% 读取点和面
% [V,F]=readObj(cathead);
% v=size(V,1);%h为点数
% f=size(F,1);%f为面数
% 
% %% 找到边界点
% B=findBoundary(V,F);
% b=size(B,2);
% 
% %% 找到内部点
% N=setdiff(1:v,B);
% n=v-b;
% 
% %% 创建边稀疏矩阵，直接存储这条线下一个连接的点
% E=zeros(v,v);
% for i=1:f
%     E(F(i,1),F(i,2))=F(i,3);
%     E(F(i,2),F(i,3))=F(i,1);
%     E(F(i,3),F(i,1))=F(i,2);
% end
% 
% 
% 
% 
% %% 构造系数矩阵
% A=zeros(v,v);
% for i=1:v
%     if ismember(i,N)==0
%         A(i,i)=1;
%         continue;
%     end
%     % 首先把邻接点逆时针排序，中心点ctr，邻接点矩阵near，对应序号序列nearp
%     ctr=V(i,:);
%     for j=1:v
%         if(E(i,j)>0)
%            near(1,:)=V(j,:);
%            break;
%         end
%     end
%     k=j;d=1;nearp(1)=j;
%     while E(i,k)~=j
%        d=d+1;
%        k=E(i,k);
%        nearp(d)=k;
%        near(d,:)=V(k,:);
%     end
%     % 求出坐标
%     l=cal_para(ctr,near,d);
%     % 给A第i行赋值
%     for j=1:d
%         A(i,nearp(j))=l(j,1);
%         A(i,i)=A(i,i)+l(j,1);
%     end
%     A(i,i)=-A(i,i);
% %     A(i,i)=-d;
% end
% m=check(A,v);
% 
% %% 解方程
% o=zeros(v,2);
% C=exp(2i*pi*(0:11)'/12);
% for i=1:b
%     o(B(1,i),1)=real(C(i));
%     o(B(1,i),2)=imag(C(i));
% end
% x(:,1)=A\o(:,1);
% x(:,2)=A\o(:,2);
% 
% %% 画图
% drawmesh(F,x);
% 
% catheadplanar='finish';
% 


