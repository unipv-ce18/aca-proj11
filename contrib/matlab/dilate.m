function out = dilate( img, elem )

% "elem" is the structural element
[h, w] = size(img);
out = zeros(h, w);

[eh, ew] = size(elem);
if mod(eh, 2) == 0 || mod(ew, 2) == 0
  error('Structural element must be odd sized');
end

heh = (eh-1)/2;
hew = (ew-1)/2;

for s = 1+heh:h-heh
  for t = 1+hew:w-hew
    parts = zeros(eh, ew);
    for p = -heh:heh
      for q = -hew:hew
        i = 1 + heh + p;
        j = 1 + hew + q;

        parts(i,j) = img(s+p,t+q) + elem(i,j);
      end
    end
    out(s,t) = max(max(parts)); 
  end
end

end
