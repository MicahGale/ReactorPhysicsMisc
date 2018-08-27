%%%22.212 Fall 2017- Written by Micah Gale

%Uses to numerically integrate a function

%Inputs:
%func-the function to be integrated
%a - lower bound of integration
%b - upper bound of integration
%tolerance- the target tolerance. Output varies by less than
%tolerance*output
%granularity- number of bins to use. default: 100

function [solved,granularity]=numInt(func,a,b,tolerance,varargin)
    if length(varargin)==1
        granularity=int(varargin(1));
    else
        granularity=100;
    end
    solved=0;
    last_run=20;
    eval=zeros(1,granularity+1);
    
    while (solved*tolerance<abs(last_run-solved))
        last_run=solved;
        solved=0;
        step_size=(b-a)/granularity;
        bounds= a:step_size:b;
        parfor j=1:length(bounds) % fill in holes on the eval matrix
            if abs(eval(j))<1e-6 %if essentially 0
                eval(j)=func(bounds(j)); %if it's zero evaluate it
            end
        end
        %use the trapezoidal rule for n runs
        for j=1:granularity 
            up=eval(j+1);
            down=eval(j);
            trap=step_size*(up+down)/2;
            solved=solved+trap;
        end
        granularity=double(granularity*2); %increase the resolution by 5 if the test fails
        newEval=zeros(1,granularity+1);
        for j=0:length(eval)-1
            newEval(2*j+1)=eval(j+1);
        end
        eval=newEval;
    end
end