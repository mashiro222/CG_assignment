function result = ARAP_interp(n, t, M, Q, theta, X)

    JX = zeros(2*n, 2);% jacobi矩阵乘差向量矩阵

    Qt = (1-t)*reshape(kron(ones(1,n),eye(2)),2,2,n)+t*Q;% 平移矩阵插值

    c = cos(t*theta); s = sin(t*theta);  % 旋转插值
    
    for i=1:n
        Rt=[c(i),-s(i);s(i),c(i)];
        Qt(:,:,i) = Rt *Qt(:,:,i);% 直接把旋转的插值乘上去
        JX(2*i-1:2*i,:) = Qt(:,:,i)*X(:,:,i);
    end

    JX1 = [JX(1:2:2*n,:);JX(2:2:2*n,:)];  % 因为设置的M是合起来设的，这边得重排一下  
    result= M\JX1;

end