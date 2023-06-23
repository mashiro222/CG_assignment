function h = shownumber(x, nums, col, fmt, varargin)

if isa(x,'gpuArray'), x = gather(x); end

if nargin<2 || isempty(nums)
    nums = 1:size(x,1);
end

if ~isreal(x), x = [real(x) imag(x)]; end

if ~isa(x, 'double'), x = double(x); end

if length(nums) ~= size(x,1)
    error('array lengths doesn''t match');
end

nums = reshape(nums, [], 1);

if nargin<3,    col = 'r'; end

if nargin>3 && ~isempty(fmt)
    strs = num2str(nums, fmt);
else
    strs = num2str(nums);
end


if size(x,2)==2
    h=text( x(:,1), x(:,2), strs ); % fixed for complex numbers on 28 May 2015
else
    h=text( x(:,1), x(:,2), x(:, 3), strs );
end

set(h, 'FontSize', 12, 'LineWidth', 1, 'Color', col, varargin{:});
