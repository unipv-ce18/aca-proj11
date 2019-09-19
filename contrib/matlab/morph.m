%% Mathematical Morphology

I = double(imread('images/horsesm.png'));

%% Base operators

SE = double(makecircle(10));
SE(SE == 0) = NaN; % NaN = don't care

subplot(2,3,1), imshow(uint8(I)), title('Original image');
subplot(2,3,4), imshow(SE), title('Structural element');
drawnow;

% Dilation
di = dilate(I, SE);
subplot(2,3,2), imshow(uint8(di)), title('Dilated image');
drawnow;

% Erosion
er = erode(I, SE);
subplot(2,3,5), imshow(uint8(er)), title('Eroded image');
drawnow;

% Opening (dilation of the erosion)
op = dilate(erode(I, SE), SE);
subplot(2,3,3), imshow(uint8(op)), title('Opening of image');
drawnow;

% Closing (erosion of the dilation)
cl = erode(dilate(I, SE), SE);
subplot(2,3,6), imshow(uint8(cl)), title('Closing of image');
drawnow;

%% Skeleton

SE = [ 0 1 0; 1 0 1; 0 1 0 ];
SE(SE == 0) = NaN; % NaN = don't care


% See also (OpenCV):
% http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/

% MATLAB does it like this:
% figure, imshow(bwmorph(I, 'skel',Inf));

nxI = I;
skel = zeros(size(I));

while true
    % Union: OR: (using Zadeh operators) MAX
    % Intersection: AND: (using Zadeh operators) MIN

    tmpI = erode(nxI, SE);
    skel = max(skel, nxI - dilate(tmpI, SE)); % <- opt for binary
    %skel = max(skel, min(nxI, 255 - dilate(tmpI, SE)));
    nxI = tmpI;

    rem = nnz(nxI > 0);
    if ~rem; break; end

    imshow(uint8(skel));
    set(gcf, 'Name', sprintf('Generating skel (%d to erode)\n', rem));
    drawnow;
end

set(gcf, 'Name', 'Morphological Skeleton');
imshow(uint8(skel));
