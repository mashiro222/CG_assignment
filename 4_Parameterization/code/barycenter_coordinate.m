function coor=barycenter_coordinate(w,w1,w2,w3)

x=w(1,1);y=w(1,2);x1=w1(1,1);y1=w1(1,2);x2=w2(1,1);y2=w2(1,2);x3=w3(1,1);y3=w3(1,2);
A=zeros(3,3);
A(1,:)=[x1-x,x2-x,x3-x];
A(2,:)=[y1-y,y2-y,y3-y];
A(3,:)=[1,1,1];
b=zeros(3,1);b(3,1)=1;

coor=A\b;% 是一个3*1的矩阵，对应三个坐标

end