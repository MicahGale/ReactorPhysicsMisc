
function edges=formLogBounds(lower, Upper, bins)
    range=Upper-lower;
    size=range/2^bins;
    edges=[lower];
    for i=0:bins-1
        edge=lower+size*2^i;
        edges=[edges;edge];
        lower=edge;
    end
end