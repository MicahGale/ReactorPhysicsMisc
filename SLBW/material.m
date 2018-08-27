

classdef material
    properties (Access=private)
        resonances %array of ressonances
        ranges      %array of bounds of the effects of each resonances
        N           %[atoms/cc]
        sig_pot     %potential sigma
        perMolecule %number of atoms in a single molecule doesn't affect N
    end
    
    methods
        function this=material(N,sig_pot,resonances,ranges,perMolecule)
            this.N=N;
            this.sig_pot=sig_pot;
            this.resonances=resonances;
            this.ranges=ranges;
            this.perMolecule=perMolecule;
        end
        function resonace=getRes(this,i)
            resonace=this.resonances(i);
        end
        function pot=getSigPot(this)
            pot=this.sig_pot;
        end
        function N=getN(this)
            N=this.N;
        end
        function pot=getSigPotWeighted(this)
            pot=this.sig_pot*this.perMolecule;
        end
        %pushs a resonance back into the array after editing. By the way
        %MATLab object orientation is bullshit, and the worst way to
        %interact with internal modifications. Ohhh java how I miss thee.
        %system.out.println() isn't even that bad to work with but alas I
        %have fallen victim to the siren's call of Mathworks, and have become
        %a cog in their corporate machinery that replaces good syntax with
        %shiny functions. 
        
        %input
        %i- the location to push it into
        %res- the new resonance object to use
        function this=pushRes(this,i,res)
            this.resonances(i)=res;
        end
        %Returns the range array for i-th peak
        function range=getRange(this,i)
            range=this.ranges(i,:);
        end
        %returns the Sigma for given E
        %INPUT
        %E- input energy
        %mode- the resonance model to use
        function xs=getMacroXS(this,E,mode)
            xs=0;
            low=(this.ranges(:,1) <=E);
            high=(this.ranges(:,2) >= E);
            range=sum(low+high,2)==2; %sums the rows of low+high if it's 2 that row is within the range
            if range==zeros(size(range)) %breaks if not in range with 0
                return
            end
            for k=1:length(range) %find right row
                if range(k)==1
                    break
                end
            end
            if strcmp(mode,'SQ')   %does the square approximation
                micro=this.resonances(k).squareRes(E);
                xs=this.N*micro;
            elseif strcmp(mode,'SA') %does the approximate SLBW
                micro=this.resonances(k).SLBWAPprox(E);
                xs=this.N*micro;
            end
        end
        %returns the sigma for given E
        %INPUT
        %E- input energy
        %mode- the resonance model to use
        function micro=getMicroXS(this,E,mode)
            low=(this.ranges(:,1) <=E);
            high=(this.ranges(:,2) >= E);
            range=sum(low+high,2)==2; %sums the rows of low+high if it's 2 that row is within the range
            for k=1:length(range) %find right row
                if range(k)==1
                    break
                end
            end
            if strcmp(mode,'SQ')   %does the square approximation
                micro=this.resonances(k).squareRes(E);
            elseif strcmp(mode,'SA') %does the approximate SLBW
                micro=this.resonances(k).SLBWAPprox(E);
            end
        end
        
        function G=getGamma(this,E)
            low=(this.ranges(:,1) <=E);
            high=(this.ranges(:,2) >= E);
            range=sum(low+high,2)==2; %sums the rows of low+high if it's 2 that row is within the range
            for i=1:length(range) %find right row
                if range(i)==1
                    break
                end
            end
            G=this.resonances(i).getG();
        end
    end
    
end