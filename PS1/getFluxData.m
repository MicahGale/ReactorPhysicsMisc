%plots the flux for a given series

function a=plotluxData(M, edges,chiPhi)
    outPut=zeros(length(edges)-1,1); %grow it to the right size
    if chiPhi
        chi=zeros(size(outPut));
        phi=zeros(size(outPut));
        sigP=zeros(size(outPut));
    end
    energy=zeros(length(edges)-1,1);
    for i=2:length(edges)
       energy(i-1)=(edges(i)+edges(i-1))/2; 
    end %get the midpoint energy
    
    for i=1:length(M) %iterate over rows
        buffer=M(i,:);
        for j=2:length(edges)
           if buffer(2) <= edges(j)&& buffer(2)>= edges(j-1) %if it falls in this bin!
               outPut(j-1)=outPut(j-1)+1/buffer(4);
               if chiPhi   %if tracking chi and Phi then do it!
                   chi(j-1)=chi(j-1)+1/buffer(4)*buffer(5); %weights the flux by chi
                   phi(j-1)=phi(j-1)+1/buffer(4)*buffer(6); %weights by phi
                   sigP(j-1)=sigP(j-1)+1/buffer(4)*buffer(7);
               end
               break;
            end
        end
    end
    a=figure('units','normalized','outerposition',[0 0 1 1]);
    hold on;
    if chiPhi
        plot(energy,chi,energy,phi,energy,sigP);
        legend('Chi Function','Phi function','Potential Scattering');
    else
        plot(energy,outPut);
    end
    ylim([0 inf]);
end