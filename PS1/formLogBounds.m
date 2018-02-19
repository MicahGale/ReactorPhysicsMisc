
function edges=formLogBounds(lower, Upper, bins)
    upperEnd=log(Upper/lower);
    size=(upperEnd)/bins;
    edges=[];
    for i=upperEnd:-size:0
        edge=Upper/exp(i);
        edges=[edges;edge];
    end
end