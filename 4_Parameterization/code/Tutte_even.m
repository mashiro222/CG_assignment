function catheadplanar = Tutte_even(cathead)

%% 读取点和面
[V,F]=readObj(cathead);
v=size(V,1);%h为点数
f=size(F,1);%f为面数

%% 找到边界点
B=findBoundary(V,F);
b=size(B,2);

%% 找到内部点
N=setdiff(1:v,B);
n=v-b;

%% 创建边稀疏矩阵
E=zeros(v,v);
for i=1:f
    E(F(i,1),F(i,2))=1;E(F(i,2),F(i,3))=1;E(F(i,1),F(i,3))=1;
    E(F(i,2),F(i,1))=1;E(F(i,3),F(i,2))=1;E(F(i,3),F(i,1))=1;
end

%% 构造系数矩阵
A=zeros(v,v);
for i=1:v
    if(ismember(i,N)==1)% 若点在内部
        num=0;
        for j=1:v
            if(E(i,j)==1)
                A(i,j)=1;
                num=num+1;
            end
        end
        A(i,i)=-num;
    else% 若点在边界
        A(i,i)=1;
    end
    
end

%% 解方程
o=zeros(v,2);
C=exp(2i*pi*(0:b-1)'/b);
for i=1:b
    o(B(1,i),1)=real(C(i));
    o(B(1,i),2)=imag(C(i));
end
x(:,1)=A\o(:,1);
x(:,2)=A\o(:,2);

%% 画图
drawmesh(F,x);
 
catheadplanar='finish';












