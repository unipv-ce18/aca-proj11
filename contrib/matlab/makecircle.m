function image = makecircle( radius, radius2 )
[c,r] = meshgrid(1:2*radius-1,1:2*radius-1);
image = (r-radius).^2 + (c-radius).^2 <= radius.^2;

if exist('radius2', 'var')
    image2 = (r-radius).^2 + (c-radius).^2 <= radius2.^2;
    image = image - image2;
end

end

