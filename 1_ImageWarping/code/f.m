%% the univariate function used in the RBF transformation
function b=f(i,x,y,d,psrc)

temp=(x-psrc(i,1))^2+(y-psrc(i,2))^2;
b=1/(temp+d);

end