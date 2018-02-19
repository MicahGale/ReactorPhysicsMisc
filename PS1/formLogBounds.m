
function edges=formLogBounds(lower, Upper, bins)
    upperEnd=log(Upper/lower);
    size=(upperEnd-1)/bins;
    edges=[lower];
    for i=upperEnd:-size:1
        edge=Upper/exp(i);
        edges=[edges;edge];
    end
end