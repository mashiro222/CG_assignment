function b=RBF(i,j,a,n,psrc,d)

b=zeros(1,2);
for it=1:n
    b(1,1)=b(1,1)+a(it,1)*f(it,i,j,d,psrc);
end
for it=1:n
    b(1,2)=b(1,2)+a(it,2)*f(it,i,j,d,psrc);
end

end