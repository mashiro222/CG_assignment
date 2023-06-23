function l=cal_para(x,y,d)% x表示中心点，y表示按照逆时针环绕x的点，d表示这些点的个数，返回该点的参数，y已经按照顺序调好了

p=[0,0];
q=zeros(d,2);
q(1,:)=[norm(x-y(1,:)),0];

angsum=0;
for i=2:d
    ang=Angle(x,y(i-1,:),y(i,:));
    angsum=angsum+ang;
    q(i,1)=norm(x-y(i,:))*cos(angsum);
    q(i,2)=norm(x-y(i,:))*sin(angsum);
end

l=zeros(d,1);
q(d+1,:)=q(1,:);
for i=1:d
  for j=1:d
      % j,i,i+1三点求重心坐标
      if(i==d && j==1)
          continue;
      end
      if(j==i || j==i+1)
          continue;
      end
      coor=barycenter_coordinate(p,q(i,:),q(i+1,:),q(j,:));
      if(min(coor)>=0)
          l(i,1)=l(i,1)+coor(1,1);
          if i==d
              k=1;
          else
              k=i+1;
          end
          l(k,1)=l(k,1)+coor(2,1);
          l(j,1)=l(j,1)+coor(3,1);
      end
  end
end

