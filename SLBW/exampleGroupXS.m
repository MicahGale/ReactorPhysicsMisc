%Micah Gale 22.012 pset-2 September 2017

%Set up real fast
clear
close all
addpath('../library'); %import library

%RUN_MODE
%0-all
%1-exact group xs
%2- exact group flux
%3-dancoff factor stuff
RUN_MODE=0;

N238=2.2e22; % [atoms/cc] U-238 density is in UO2
NO16=2*N238; %[atoms/cc] is UO2
sigPU=11.4;  %[b]
sigPO=4;     %[b]
slabW=0.5;   %[cm]  width of the infinite slab

squareW=2;   %[] width of square resonance in Gamma
deltaE=6;     %[eV] width of energy group

E0=1e3;
peak=resonance(E0,0,0.05,238,11.4);
%E0=1Ke, Gamma=0.05, sigma_p-238=11.4b

%%%%%%%%%%%%%%Slab setup %%%%%%%%%%%%%%%%%%%%%
U=material(N238,sigPU,[peak],[E0-deltaE/2, E0+deltaE/2],1);
O=material(NO16,sigPO,[],[],2);                  %Oxygen has "no" resonances
slab=infinSlab(slabW,[U,O],[true,false]);                   %initializes the slab

clear N238 NO16 sigPU sigPO slabW U O
%%%%%%%%%%%%%%%%%%%%%%%%%%%Problem 1: Group XS%%%%%%%%%%%%%%%%%%%%%%%%%%%

if RUN_MODE <=1
    disp('Calculate group cross-section');
    peakSigma=[1e5, 1e4, 5e3,1e3,750, 500, 375, 250, 175, 100,...
        75, 50, 25, 10, 5, 2, 0.5]; %lots of values to calculate
    P1=texTable({'Peak Square-resonance cross-section','Group Capture cross-section; SLBW(b)','Group capture cross-section; Square Ressonance(b)'},true,'{|c|c|c|}');
    range=[E0-deltaE/2, E0+deltaE/2];
    for j=peakSigma
        slab=slab.initSqrSLBW(1,1,deltaE,j); %modifies the 1st peak of Uranium
        %updates the Square and slbw approximation for the new height
        SLBW=slab.getExactGroupXS(range,'SA',0.001);
        square=slab.getExactGroupXS(range,'SQ',0.001);
        P1=P1.addData({num2str(j,'%3.2g'),num2str(SLBW,'%5.6g'),num2str(square,'%5.6g')});
        disp(strcat('Finished: ',num2str(j)));
    end
    P1.writeTable('exactGsigC.tex');
    clear peakSigma P1 SLBW square
end
  %%%%%%%%%%%%%%%%%%%%%%Problem 2: group flux %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
if RUN_MODE ==0 || RUN_MODE==2
    disp('Calculate group averaged flux');
    peakSigma=[1e5, 1e4, 5e3,1e3,750, 500, 375, 250, 175, 100,...
        75, 50, 25, 10, 5, 2, 0.5]; %lots of values to calculate
    P2=texTable({'Peak Square-resonance cross-section','Group-averaged-flux; SLBW','Group-averaged flux; Square Ressonance(b)'},true,'{|c|c|c|}');
    range=[E0-deltaE/2, E0+deltaE/2];
    for j=peakSigma
        %first calculate the SLBW flux ave =1/range*int(phi) over range
        slab=slab.initSqrSLBW(1,1,deltaE,j); %init that stuff
        [a,slab]=slab.getFlux('SA',1); %initialize memo
        fluxInt=numInt(@(E) slab.getFlux('SA',E),range(1),range(2),0.001);
        SLBW=fluxInt/deltaE;                 %calculates the average

        
        %now do for the square
        fluxInt=slab.getFlux('SQ',mean(range))*squareW*slab.getMaterial(1).getGamma(mean(range));
        %calculate the area of the flux in the square well
        fluxInt=fluxInt+1*(deltaE-squareW*slab.getMaterial(1).getGamma(mean(range))); %add the area of everything else
        square=fluxInt/deltaE;
        
        %finish up this iteration
        P2=P2.addData({num2str(j,'%3.2g'),num2str(SLBW,'%5.6g'),num2str(square,'%5.6g')});
        disp(strcat('Finished: ',num2str(j)));
    end
    P2.writeTable('P2ExactFlux.tex');
    clear P2 SLBW rangeWidth fluxInt a
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Problem 3: dancoff factor stuff%%%%%%%%%%%
if RUN_MODE==0|| RUN_MODE==3
     disp('Calculate wigner bell approximate xs');
    peakSigma=[1e5, 1e4, 5e3,1e3,750, 500, 375, 250, 175, 100,...
        75, 50, 25, 10, 5, 2, 0.5]; %lots of values to calculate
    P3=texTable({'Peak Square-resonance cross-section','SLBW:Wigner-Bell cross-section(b)','SLBW:Wigner Bell error','Square:Wigner Bell cross-section(b)','Square:wigner Bell error'},true,'{|c|c|c|c|c|}');
    range=[E0-deltaE/2, E0+deltaE/2];
    for j=peakSigma
        slab=slab.initSqrSLBW(1,1,deltaE,j);
        SLBWWigner=slab.WignerBellxs(range,'SA',1.1,0.001,'WB');
        SquareWigner=slab.WignerBellxs(range,'SQ',1.1,0.001,'WB');
        SLBWexact=slab.getExactGroupXS(range,'SA',0.001);
        SquareExact=slab.getExactGroupXS(range,'SQ',0.001);
        SquareError=(SquareWigner-SquareExact)/SquareExact*100; %calculate percent error
        SLBWError=(SLBWWigner-SLBWexact)/SLBWexact*100;
        P3=P3.addData({num2str(j,'%3.2g'),...
                        num2str(SLBWWigner,'%5.6g'),...
                        strcat(num2str(SLBWError,'%5.6g'),'\%'),...
                        num2str(SquareWigner,'%5.6g'),...
                        strcat(num2str(SquareError,'%5.6g'),'\%')});
        disp(strcat('finished: ',num2str(j)));
    end
    P3.writeTable('P3WignerBell.tex');
    clear SLBWWigner SquareExact SquareWigner SLBWexact P2
end


%%%%%%%%%%%%%%%%%%%%%Problem 4: Carlvik/Roman%%%%%%%%%%%%%%%%%%%%%%
if RUN_MODE==0 || RUN_MODE==4
    disp('Calculate Carlvik Roman approximate cross section');
    peakSigma=[1e5, 1e4, 5e3,1e3,750, 500, 375, 250, 175, 100,...
        75, 50, 25, 10, 5, 2, 0.5]; %lots of values to calculate
    P4=texTable({'Peak Square-resonance cross-section','SLBW:Wigner-Bell cross-section(b)','SLBW:Wigner Bell error','Square:Wigner Bell cross-section(b)','Square:wigner Bell error'},true,'{|c|c|c|c|c|}');
    range=[E0-deltaE/2, E0+deltaE/2];
    alpha1=1.4;
    alpha2=5.4;
    beta=1.1;
    for j=peakSigma
        slab=slab.initSqrSLBW(1,1,deltaE,j);
        SLBWWigner=slab.WignerBellxs(range,'SA',1.1,0.001,'CR');
        SquareWigner=slab.WignerBellxs(range,'SQ',1.1,0.001,'CR');
        SLBWexact=slab.getExactGroupXS(range,'SA',0.001);
        SquareExact=slab.getExactGroupXS(range,'SQ',0.001);
        SquareError=(SquareWigner-SquareExact)/SquareExact*100; %calculate percent error
        SLBWError=(SLBWWigner-SLBWexact)/SLBWexact*100;
        P4=P4.addData({num2str(j,'%3.2g'),...
                        num2str(SLBWWigner,'%5.6g'),...
                        strcat(num2str(SLBWError,'%5.6g'),'\%'),...
                        num2str(SquareWigner,'%5.6g'),...
                        strcat(num2str(SquareError,'%5.6g'),'\%')});
        disp(strcat('finished: ',num2str(j)));
    end
    P4.writeTable('P4CarlvikRoman.tex');
end
