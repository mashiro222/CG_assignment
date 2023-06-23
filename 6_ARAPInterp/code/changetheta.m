function thetafinish=changetheta(theta,t,n)
%% bonus内容
handle = 1;% handle:正在操作三角形，从第一个开始

flag = zeros(n, 1);% 用来记录每个三角形处理过了没有，如果处理了就在对应位置存储1
flag(handle) = 1;
while handle    % 还有要操作三角形时继续
    num=sum(t==t(handle(1),1),2)+sum(t==t(handle(1),2),2)+sum(t==t(handle(1),3),2);% 找到与B(1)三角形的三个顶点有交的三角形
    adjacent = find(num==2)'; % 找到t中与操作三角形有相同边的所有三角形，用行的形式存储编号
    adjacent(flag(adjacent)==1)=[];%之前已经操作过的三角形就不再处理
    for i=adjacent
        while theta(handle(1)) - theta(i) > pi
            theta(i) = theta(i) + 2*pi;
        end
        while theta(handle(1)) - theta(i) < -pi
            theta(i) = theta(i) - 2*pi;
        end
    end% 这样相邻三角形的旋转角之间就是连续的，差值在-pi到pi之间
    flag(adjacent)=1;% 标记处理过的三角形
    
    handle=[handle,adjacent]; handle(1)=[];%排列等待操作
end


thetafinish=theta;


end