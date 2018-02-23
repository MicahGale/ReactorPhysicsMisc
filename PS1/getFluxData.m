%plots the flux for a given series

function a=plotluxData(M, edges,chi,phi)
    outPut=zeros(length(edges)-1,1); %grow it to the right size
    energy=zeros(length(edges)-1,1);
    for i=2:length(edges)
       energy(i-1)=(edges(i)+edges(i-1))/2; 
    end %get the midpoint energy
    
    for i=1:length(M) %iterate over rows
        buffer=M(i,:);
        for j=2:length(edges)
           if buffer(2) <= edges(j)&& buffer(2)>= edges(j-1) %if it falls in this bin!
               outPut(j-1)=outPut(j-1)+1/buffer(4);
               break;
            end
        end
    end
    a=plot(energy,outPut);
end