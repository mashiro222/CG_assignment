function angle=Angle(x,y,z)% y-x和z-x之间的夹角的绝对值

a2 = norm(y-x);
b2 = norm(z-x);
c2 = norm(y-z);
a = sqrt(a2);
b = sqrt(b2);
pos = (a2+b2-c2)/(2*a*b);    

angle = acos(pos);         

end