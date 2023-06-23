function m=check(A,v)
m='A的内部点对应的行求和为0，且非对角元全为正';
for i=1:v
    sum=0;
    for j=1:v
        sum=sum+A(i,j);
        if i~=j && A(i,j)<0
            m='算法有问题'
        end
    end
    if abs(sum)>1e-6 && sum~=1
        m='算法有问题'
    end
end

m
end