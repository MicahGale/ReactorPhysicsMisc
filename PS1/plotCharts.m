%Plots all the data.
%
%Forgive me Travis, for I have sinned.
%
%filenames={"Q1_0K.csv","Q1_1000K.csv"};
clear;
close all;
Q1=true;
Q2=false;
Q3=false;

if Q1
    filenames={"Q1_0K.csv"};
    for file=filenames
       M=csvread(file{1},1,0); %parses the csv file
       
       semilogy(M(:,1),M(:,2));
       hold on;
       semilogy(M(:,1),M(:,3));
       semilogy(M(:,1),M(:,2)+M(:,3));
       xlabel('Energy [eV]');
       ylabel('Cross-section [b]');
       title('Cross Sections for U-238 for first 3 resonances');
       legend('(n,gamma)','(n,n)','total');
    end
end