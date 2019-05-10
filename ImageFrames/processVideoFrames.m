for m=1:length(h)
    x = imread('../imageStitchingTest.lau','index',m);
    x = double(x);
    x = x(1:2:end,1:2:end,:);

    g = rgb2gray(x);

    x(:,1:99,1) = g(:,1:99);
    x(:,1:99,2) = g(:,1:99);
    x(:,1:99,3) = g(:,1:99);

    x(:,105:end,1) = g(:,105:end);
    x(:,105:end,2) = g(:,105:end);
    x(:,105:end,3) = g(:,105:end);

    if (m < 10)
        imwrite(x, ['frame00' num2str(m) '.tif']);
    elseif (m<100)
        imwrite(x, ['frame0' num2str(m) '.tif']);
    else
        imwrite(x, ['frame' num2str(m) '.tif']);
    end;
end;
