%Plots all the data.
%
%Forgive me Travis, for I have sinned.
%

clear;
close all;
Q1=false;
Q2=true;
Q3=false;

if Q1
    filenames={"Q1_0K.csv","Q1_1000.csv"};
    %filenames={"Q1_1000.csv"};
    for file=filenames
       M=csvread(file{1},1,0); %parses the csv file
       
       semilogy(M(:,1),M(:,2));
       hold on;
       semilogy(M(:,1),M(:,3));
       a=semilogy(M(:,1),M(:,2)+M(:,3));
       xlabel('Energy [eV]');
       ylabel('Cross-section [b]');
       title('Cross Sections for U-238 for first 3 resonances');
       legend('(n,gamma)','(n,n)','total');
       hold off;
       saveas(a,strcat(file{1},".png"),"png");
    end
end

if Q2
    M=csvread("Q2flux.csv",1,0);
    figure('units','normalized','outerposition',[0 0 1 1]);
    hold on;
    %equal energy bins
    edges=1:10:1e3; %makes 100 equal bins
    a=getFluxData(M,edges,false,false);
    xlabel('Energy in equal bins [eV]');
    ylabel('Flux');
    title('Thermalization of a 1KeV source in a pure scattering medium');
    saveas(a,"Q2FluxEqual.png","png");
    hold off;
    upper=1e3;
    lower=1;
    bins=200;
    
    %equal lethergy bins
    edges=formLogBounds(lower,upper,bins);
    a=getFluxData(M,edges,false,false);
    xlabel('Energy in equal lethergy bins [eV]');
    ylabel('Flux');
    title('Thermalization of a 1KeV source in a pure scattering medium');
    saveas(a,"Q2FluxLethergy.png","png");
    
    %first second third collisons
    first=M(M(:,1)==1,:); 
    second=M(M(:,1)==2,:);
    third=M(M(:,1)==3,:); %the first second and third collisions
    c=figure('units','normalized','outerposition',[0 0 1 1]);
    hold on;
    histogram(first);
    histogram(second);
    histogram(third);
    h = findobj(gca,'Type','patch');
    set(h,'facealpha',0.5);
    set(gca, 'YScale', 'log');
    xlabel('energy [eV]');
    ylabel('flux');
    legend('first collision','second collison','third collison');
    saveas(c,"Q2FirstCollide.png","png");

end

if Q3
   fileNames={'Q3Flux_0K_10to1.csv','Q3Flux_0K_1000to1.csv','Q3Flux_0K_1e+06to1.csv'...c
       'Q3Flux_1000K_10to1.csv','Q3Flux_1000K_1000to1.csv','Q3Flux_1000K_1e+06to1.csv'}; 
    upper=1e3;
    lower=1;
    bins=100;
    edges=formLogBounds(lower,upper,bins);
   for file=fileNames
       M=csvread(file{1},1,0);
       file{1} %print to the terminal
       absorb=M(M(:,3)==0,:);
       length(absorb)/90e3
       a=getFluxData(M,edges,false,false);
       set(gca,'XScale','log');
       xlabel('energy [eV]');
       ylabel('flux');
       title('Flux in an infinite homogenous medium of U-238 and H-1');
       saveas(a,strcat(file{1},".png"),"png");
       hold off;
   end
end