%Micah Gale 22.012 pset-1 September 2017

%Set up real fast
clear
close all
addpath('../library'); %import library

%Run mode:
%0-all
%1- plot xs only
%2- solve RI's
%3- NR and WR RI's
%4- doppler on Infinity-dilute RI
%5- doppler on infin-dilute XS
%6- Doppler NR infin RI

%NOTE: mode 6 is currently barely functional, and very inefficient use
%caution in RUN_MODE 0 or 6
RUN_MODE=6;

%%%%%%%%%Load Data from Pset%%%%%%%%


A=238; 
sig_pot=9.28;

peaks=[ resonance(6.67,0.00148,0.023,A,sig_pot), ...
        resonance(20.87,0.01009,0.02286,A,sig_pot), ...
        resonance(36.68,0.03355,0.02300,A,sig_pot)];


ranges = [ 6, 10; 10, 25; 25, 50]; %the ranges to calculate the RI over

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Question 1%%%%%%%%%%%%%%%%%%%%%%%%%%%
%plot the xs
if RUN_MODE<=1
    X=0.01:0.001:100;
    Y=zeros(size(X));
    for k=peaks
        buffer=k.SLBW();
        Y=Y+buffer(X);
    end
    loglog(X,Y);
    axis([1e-2, 100,1e-3, 1e5]);
    ylabel('\sigma_c (b)');
    xlabel('Energy (eV)');
    title('Uranium-238 Microscopic Capture Cross-Section');
    saveas(gcf,'XSU238.png');
    saveas(gcf,'XSU238.fig');
    clear X Y buffer
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Question 1???%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Question 2 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if RUN_MODE==0 || RUN_MODE==2
    disp('Calculates the 0K infinite-dilute Resonance-Integral');
    RI= texTable({'Energy Range (eV)','Resonance Integral'},true,'[th]{|c|c|}');
    %cycle through the peaks
    for k=1:length(peaks)
        RangeTerm=strcat(num2str(ranges(k,1),2),'-',num2str(ranges(k,2),2));
        Rint=num2str(peaks(k).getRIInfin(ranges(k,1),ranges(k,2),0.00001),'%5.2f');
        RI=RI.addData({RangeTerm,Rint});
        fprintf('%s    %s\n',RangeTerm,Rint);
    end
    RI.writeTable('Q1RIInfin.tex');
    clear RI RangeTerm Rint
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Question4: NR/WR%%%%%%%%%%%%%

if RUN_MODE==0 ||RUN_MODE==3
   disp('Calculates the NR and WR ressonance integrals at 0K');
   a=6;
   b=50;
   sig_b=[2e6,2e4,2e3,2e2,2e1];
   NR_WR=texTable({'$\sigma_{background}$(b)','$RI_{NR}$','$RI_{WR}$'}...
       ,true,'[bh]{|c|c|c|}');
   for k=1:length(sig_b)   %go through all the background dilutions
       NRsum=0;
       WRsum=0;
       sig_disp=num2str(sig_b(k),2);
       for j=1:length(peaks)
          NRsum=NRsum+peaks(j).getRINR(sig_b(k),a,b,0.0001);
          WRsum=WRsum+peaks(j).getRIWR(sig_b(k),a,b,0.0001);
       end
       NR=num2str(NRsum,'%5.2f');
       WR=num2str(WRsum,'%5.2f');
       NR_WR=NR_WR.addData({sig_disp,NR,WR});
       fprintf('%s         %s           %s\n',sig_disp,NR,WR);
   end
   NR_WR.writeTable('Q4NRWR.tex');
   clear a b sig_b NRsum NR WR WRsum NR_WR
end

%%%%%%%%%%%%%%%%%%%%%%%%%%Question 5: doppler broadening%%%%%%
if RUN_MODE==0 || RUN_MODE==4
    T=[3, 300]; %temperatures to test
    
    doppler=texTable({'Energy Range (eV)','RI 3K','RI 300K'},true,'[hb]{|c|c|c|}');
    for k=1:length(peaks)
        sum=zeros(size(T));
        RangeTerm=strcat(num2str(ranges(k,1),2),'-',num2str(ranges(k,2),2));
        for j=1:length(T)
            sum(j)=peaks(k).getRIInfinDoppler(T(j),ranges(k,1),ranges(k,2),0.01);
        end
        doppler=doppler.addData({RangeTerm,num2str(sum(1),'%5.2f'),num2str(sum(2),'%5.2f')});
    end
    doppler.writeTable('dopplerRI.tex');
end

%%%%%%%%%%%%%%%%%%%%%%Question 6: Doppler group XS%%%%%%%%%%%%%%%%%%%%

if RUN_MODE==0 || RUN_MODE==5
    T=[3, 300]; %temperatures to test
    
    doppler=texTable({'Energy Range (eV)','$\sigma_c$ 3K (b)','$\sigma_c$ 300K (b)'},true,'[h]{|c|c|c|}');
    for k=1:length(peaks)
        sum=zeros(size(T));
        RangeTerm=strcat(num2str(ranges(k,1),2),'-',num2str(ranges(k,2),2));
        for j=1:length(T)
            sum(j)=peaks(k).getInfinXS(T(j),ranges(k,1),ranges(k,2),0.001);
        end
        doppler=doppler.addData({RangeTerm,num2str(sum(1),'%5.2f'),num2str(sum(2),'%5.2f')});
    end
    doppler.writeTable('dopplerXS.tex');
end

%%%%%%%%%%%%%%%%%%%%%%Question 7: Doppler NR RI%%%%%%%%%%%%%%
if RUN_MODE==0 || RUN_MODE==6
   disp('Calculates the NR and WR ressonance integrals at varying T');
   a=6;
   b=50;
   sig_b=[2e6,2e4,2e3,2e2,2e1];
   T=[0.25, 300,1000,1e4];
   NRDop=texTable({'$\sigma_{background}$(b)','0.25K$',...
       '300K','1000K','10,000K'}...
       ,true,'[th]{|c|c|c|c|c|}');
   for k=1:length(sig_b)   %go through all the background dilutions
       sum=zeros(size(T));
       sig_disp=strcat('\textbf{',num2str(sig_b(k),2),'}');
       for l=1:length(T)
           for j=1:length(peaks)
              sum(l)=sum(l)+peaks(j).getRINRDoppler(T(l),sig_b(k),a,b,0.0001)
           end
       end
       out={};
       for l=1:length(T)
           out={out,num2str(sum(l),'%5.2f')};
       end
       NRDop=NRDop.addData({sig_disp,out});
       %fprintf('%s         %s           %s\n',sig_disp,NR,WR);
   end
   NRDop.writeTable('NRDop.tex');
   clear a b sig_b T sm NRDop out
end

