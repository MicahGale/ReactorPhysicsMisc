
%creates an object for an infinite slab reactor.
classdef infinSlab
    properties (Access= private)
        W %width of the slab
        materials
        fuel %boolean array of which parts are marked as fuel
        %memoized objects
        memoPE
        sigTF
    end
    methods
        
        %constructor
        %slabW- the width of the slab in [cm]
        %materials- array of material objects in the slab
        function this=infinSlab(slabW,materials,fuel)
            this.W=slabW;
            this.materials=materials;
            this.fuel=fuel;
        end
        %Returns the escape probability given a Macro-xs
        %you need to calculate in the energy dependance yourself
        
        function out=escapeP(this, Sigma)
            b=this.W*Sigma;
            out=1/(2*b);
            out=out*(1-(exp(-b)-b*(exp(-b)-exp(-b))));
        end
        %Returns the requested material object
        function mat=getMaterial(this,i)
            mat=this.materials(i);
        end
        %Finds the fuel-to-fuel probability with the wigner approximation
        %INPUTS:
        %mode- the resonance mode 'SQ'=square 'SA'=approx SLBW
        %E- energy evaluation point
        function out=WignerBellFF(this,mode,E,bellFactor)
            for i=1:length(this.fuel)
                if this.fuel(i)==1 %if is capture fuel add it in
                    sigPfuel=this.materials(i).getSigPot();
                    Nfuel=this.materials(i).getN();
                    break %only handle one fuel
                end
            end
            sigT= sigPfuel+this.materials(i).getMicroXS(E,mode);
            out=sigT/(bellFactor/(this.W*Nfuel)+sigT);
        end
        
        %note only does slab geometry
        function out=CarlvikRomanFF(this,mode,E)
            for i=1:length(this.fuel)
                if this.fuel(i)==1 %if is capture fuel add it in
                    sigPfuel=this.materials(i).getSigPot();
                    break %only handle one fuel
                end
            end
            sigT= sigPfuel+this.materials(i).getMicroXS(E,mode);
            out=1-(1.1*1.4/(sigT*this.W+1.4)-0.1*5.4/(sigT*this.W+5.4));
        end
        %Finds the group cross-section using the wigner bell rational
        %approximation
        
        function xs=WignerBellxs(this,range,mode,bellFactor,tolerance,bellRoman)
            if strcmp(bellRoman,'WB')
                pFF= @(E) this.WignerBellFF(mode,E,bellFactor);
            elseif strcmp(bellRoman,'CR')
                pFF=@(E) this.CarlvikRomanFF(mode,E);
            end
            
            for i=1:length(this.fuel)
                if this.fuel(i)==1 %if is capture fuel add it in
                    sigPfuel=this.materials(i).getSigPot();
                    Nfuel=this.materials(i).getN();
                    break %only handle one fuel
                end
            end
            
            %find the flux approximate f
            sigTFuel=@(E) Nfuel*(sigPfuel+this.materials(i).getMicroXS(E,mode));
            flux= @(E) (sigTFuel(E)*pFF(E)+sigTFuel(E)*(1-pFF(E)))/sigTFuel(E);
            fluxInt=numInt(@(E) flux(E)*1/E,range(1),range(2),tolerance);
            topInt=numInt(@(E) this.materials(i).getMicroXS(E,mode)*flux(E)*1/E,...
                range(1),range(2),tolerance);
            xs=topInt/fluxInt;
        end
        %finds the exact group cross-section for the given range
        %INPUTS
        %Range - array of the upper and lower bounds to do
        %mode- the resonance model to use
            %SQ=square
            %SA- approximate SLBW
        
        function [xs,this]=getExactGroupXS(this,range,mode,tolerance)
            for i=1:length(this.fuel)
                if this.fuel(i)==1 %if is capture fuel add it in
                    pE=@(E) this.escapeP(this.materials(i).getMacroXS(E,mode));
                    this.materials(i).getMacroXS(1002,mode);
                    break %only handle one fuel
                end
            end
            if strcmp(mode,'SA') %for the approximate SLBW model
                %flux stuff
                [a,this]=this.getFlux(mode,1); %save the memoized version
                flux =memoize (@(E) this.getFlux(mode,E));
                flux.CacheSize=1e5; %big flux cache size
                fluxInt=numInt(flux,range(1),range(2),tolerance);
                xs=numInt(@(E) this.materials(i).getMicroXS(E,'SA')*flux(E),...
                    range(1),range(2),tolerance)/fluxInt;
            elseif strcmp(mode,'SQ')
                capXS=this.materials(i).getMicroXS(mean(range),'SQ'); %get square peak sig_c
                flux=@(E) this.getFlux(mode,E);
                Gamma=this.materials(i).getGamma(mean(range));
                xs=(2*Gamma*capXS*flux(mean(range)))/(1*(mean(range)-2*Gamma)+2*Gamma*flux(mean(range)));
            end
        end
        %returns the flux level for the requested energy
        %INPUTS
        %Range - array of the upper and lower bounds to do
        %mode- the resonance model to use
            %SQ=square
            %SA- approximate SLBW
        function [flux,this]=getFlux(this,mode,E)
             %setup escape prob function.
            flux=0;
            for i=1:length(this.fuel)
                if this.fuel(i)==1 %if is capture fuel add it in
                    pE=@(E) this.escapeP(this.materials(i).getMacroXS(E,mode));
                    this.materials(i).getMacroXS(1002,mode);
                    break %only handle one fuel
                end
            end
            this.memoPE=memoize(pE);
            this.memoPE.CacheSize=1e3;
            pFF = @(E) 1-this.memoPE(E); %get the fuel-to-fuel from p(escape)
            sigP=this.materials(i).getSigPot(); %gets the fuel sig_p
            if strcmp(mode,'SA') %for the approximate SLBW model
                this.sigTF=memoize(@(E) sigP+this.materials(i).getMicroXS(E,'SA'));
                this.sigTF.CacheSize=1e4; %up the cache size
                
                %flux stuff
                flux =(sigP*pFF(E)+this.sigTF(E)*this.memoPE(E))/this.sigTF(E);
            elseif strcmp(mode,'SQ')
                capXS=this.materials(i).getMicroXS(E,'SQ'); %get square peak sig_c
                flux=(sigP*pFF(E)+capXS*this.memoPE(E))/capXS;
            end
        end
        %initializes the jth peak of the ith material with a new square
        %peak-peak, then readies the SLBW approximation for this
        %INPUTS
        %i- material to play with
        %j- the resonance to play with
        %width- the width of the square peak in Gamma
        %peak- the sigma_peak of the square res opproximation
        function this=initSqrSLBW(this,i,j,width,peak)
            mat=this.materials(i); %pull out the material to play with
            res=mat.getRes(j);
            res=res.initializeSquare(width,peak); %shove in the square-peak values
            res=res.initSLBWApprox(width);
            this.materials(i)=this.materials(i).pushRes(j,res);        %shove everything back into their
            clear this.memoPE this.sigTF  %clears out the cache to allow new functions
            %places, becuase you know that's how Object-orientation works
            %right? 
        end
        function xs=totalSigPot(this)
            xs=0;
            for i=this.materials  %goes over all materials
                xs=xs+i.getSigPotWeighted();
            end
        end
    end
end