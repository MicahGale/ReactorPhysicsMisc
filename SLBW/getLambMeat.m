

function meat=getLambMeat(squiggle,E,E0,G)
    persistent memo
    X= 2*(E-E0)/G;
    local= @(squiggle,X) squiggle*lambertw((X+1i)*squiggle);
    memo=memoize(local);
    memo.CacheSize=1e5;
    meat=memo(squiggle,X);
    
end