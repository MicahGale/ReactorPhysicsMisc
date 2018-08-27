%%%22.212 Fall 2017- Written by Micah Gale1

%A class for handling nuclear resonances

classdef resonance
    properties (Access= private)
        E0
        Gn
        Gg
        G
        R0
        A
        K
        sig_pot %the potential cross-section 
        lambMeat %a memoized object of the lambertw part of the psi and 
                   %chi functions
        squareWidth %properties for square resonance approximation
        squarePeak
        SLBWPeak %peak xs for the SLBW case sig=peak*(1/(1+x^2))
    end
    
    methods
        function this=resonance(E0,Gn,Gg,A,pot)
            this.E0=E0;
            this.Gn=Gn;
            this.Gg=Gg;
            this.G=this.Gn+this.Gg;
            this.A=A;
            this.R0=2603911/this.E0*((this.A+1)/this.A)^2;
            this.sig_pot=pot;
            this.K=8.6173303e-5;
        end
        %Returns an anonymous function for the single-level breight-wigner 
        %resonance model for sigma_gamma

        % Inputs: all taken from loaded values
        % E0- the energy of the resonance
        % Gn- the gamma(FWHM) of the (n,n) rxn
        % Gg- the gamma(FWHM) of the (n,g) rxn
        % A-  the target nucleas A number
        % 
        % 

        function sig_g=SLBW(this)
            sig_g= @(E) sqrt(this.E0./E).*this.R0.*(this.Gn.*this.Gg)./(this.G.^2)*1./(1+((2.*(E-this.E0))/this.G).^2);
        end
        
        function out=SLBWAPprox(this,E)
            x=2*(E-this.E0)/this.G;
            out=this.SLBWPeak.*(1./(1+x.^2)); 
        end
        
        %returns an anonymous function for the SLBW sigma_n function
        %makes approximations for chi and psi that are valid for large xi
        function sig_n=getSigN(this)
            x=@(E) 2*(E-this.E0)/this.G;
            sig_n= @(E) this.R0*this.Gn^2/this.G^2*1/(1+x(E)^2+2*sqrt(this.sig_pot/this.R0)*this.G/this.Gn*x(E)/(1+x(E)^2))+this.sig_pot;
        end
        
        %returns an anonymous function for the SLBW sigma_n function with
        %doppler
        function sig_n=getSigNDoppler(this,T)
            chi=this.getChi(T);
            psi=this.getPsi(T);
            sig_n= @(E) this.R0*this.Gn^2/this.G^2*(psi(E)+2*sqrt(this.sig_pot/this.R0)*this.G/this.Gn*chi(E))+this.sig_pot ;
        end
        
        %returns the value for xi(squiggle) for a given temeperature
        %INPUTS
        %T- the temperature in Kelvin
        function xi=getSquiggle(this,T)
           xi=this.G/2*sqrt(this.A/(4*this.K*T*this.E0)); 
        end
        
        %returns an anonymous function for the SLBW model with Doppler
        %broadening
        
        function SLBW=getDopplerSLBW(this,T)
            psi=this.getPsi(T);
            SLBW= @(E) sqrt(this.E0/E)*this.R0*this.Gn*this.Gg/this.G^2*psi(E);
        end
        %returns an anonymous function for psi
        
        %INPUTS:
        %T- temperature in K
        
        function psi=getPsi(this,T)
            squiggle=this.getSquiggle(T); 
            psi=@(E) sqrt(pi())*real(this.lambMeat(squiggle,E));
        end
        
        %returns an anonymous function for chi
        
        %INPUTS:
        %T- temperature in K
        
        function psi=getChi(this,T)
            squiggle=this.getSquiggle(T); 
            psi=@(E) sqrt(pi())*imag(this.lambMeat(squiggle,E));
        end
        
        %returns the capture xs using a square resonance. Must initialize
        %first though
        %INPUTS
        %E- energy to eval
        function out=squareRes(this,E)
            out=0; %defaults to nothing
            if abs(E-this.E0)<(this.squareWidth*this.G)/2 %if inside square region
                out=this.squarePeak;
            end
        end
        
        
        %calculates the ressonance integral for this peak alone for the
        %infinitely dilute case.
        
        %INPUTS:
        %a- the lower bound of energy to integrate
        %b- the upper bound of energy to integrate
        %tolerance- the amount of tolerance in the numerical integration
        
        function sum=getRIInfin(this,a,b,tolerance)
            SLBW=this.SLBW();
            integrand= @(E)  SLBW(E)*1/E;
            sum = numInt(integrand,a,b,tolerance);
        end
        
        %calculates the ressonance integral for this peak alone for the
        %infinitely dilute case at a given temperature T
        
        %INPUTS:
        %T- temperature in K
        %a- the lower bound of energy to integrate
        %b- the upper bound of energy to integrate
        %tolerance- the amount of tolerance in the numerical integration
        
        function sum=getRIInfinDoppler(this,T,a,b,tolerance)
            SLBW=this.getDopplerSLBW(T);
            integrand= @(E)  SLBW(E)*1/E;
            sum = numInt(integrand,a,b,tolerance);
        end
        %calculates the ressonance integral for this peak alone using the
        %narrow resonance model
        
        %INPUTS:
        %sig_b- the background cross-section=Nm*sigm/Nr
        %a- the lower bound of energy to integrate
        %b- the upper bound of energy to integrate
        %tolerance- the amount of tolerance in the numerical integration
        
        function sum=getRINR(this,sig_b,a,b,tolerance)
            SLBW=this.SLBW();
            sig_n=this.getSigN(); %get the scattering xs
            sig_t= @(E) SLBW(E)+sig_n(E); %get the total xs
            integrand= @(E)  SLBW(E)/(sig_t(E)+sig_b)*1/E;
            sum = (this.sig_pot+sig_b)*numInt(integrand,a,b,tolerance);
        end
        %calculates the ressonance integral for this peak alone using the
        %wide resonance model
        
        %INPUTS:
        %sig_b- the background cross-section=Nm*sigm/Nr
        %a- the lower bound of energy to integrate
        %b- the upper bound of energy to integrate
        %tolerance- the amount of tolerance in the numerical integration
        
        function sum=getRIWR(this,sig_b,a,b,tolerance)
            SLBW=this.SLBW();
            integrand= @(E)  SLBW(E)*sig_b/(SLBW(E)+sig_b)*1/E;
            sum = numInt(integrand,a,b,tolerance);
        end
        
        %returns the infintely-dilute capture cross-section with doppler
        %broadening.
        
        %INPUTS:
        %T-temperature in K
        %a- lower bound [eV]
        %b-uppper bound
        %tolerance
        function sum=getInfinXS(this,T,a,b,tolerance)
            flux=numInt(@(E) 1/E,a,b,tolerance);
            sum=this.getRIInfinDoppler(T,a,b,tolerance)/flux;
        end
        %calculates the ressonance integral for this peak alone using the
        %narrow resonance model and doppler broadening
        
        %INPUTS:
        % T- the new temperature in K
        %sig_b- the background cross-section=Nm*sigm/Nr
        %a- the lower bound of energy to integrate
        %b- the upper bound of energy to integrate
        %tolerance- the amount of tolerance in the numerical integration
        
        function sum=getRINRDoppler(this,T,sig_b,a,b,tolerance)
            SLBW=this.getDopplerSLBW(T);
            sig_n=this.getSigNDoppler(T); %get the scattering xs
            sig_t= @(E) SLBW(E)+sig_n(E); %get the total xs
            integrand= @(E)  SLBW(E)/(sig_t(E)+sig_b)*1/E;
            sum = (this.sig_pot+sig_b)*numInt(integrand,a,b,tolerance);
        end
        
        %initializes the memomized function for the lambertW term in psi
        %and chi. Must be called before the psi, and chi function.
        %size- is the cachesize of the memoized function. Should be close
        %to expected number of unique function calls
        function this=initializeLamb(this,size) 
            X= @(E) 2*(E-this.E0)/this.G;
            local= @(squiggle,E) squiggle*lambertw((X(E)+1i)*squiggle);
            this.lambMeat=memoized(local);
            this.lambMeat.CacheSize=size;
        end
        
        %initialize the square resonance approximation so squareRes can be
        %called properly
        
        %Inputs- 
        %width- unitless input, is measured in gammas 
        %peak- value of the peak xs
        function this=initializeSquare(this,width,peak)
            this.squareWidth=width;
            this.squarePeak=peak; %find the peak using a=h*w
        end
        
        %finds the peak xs for the SLBW approximation that will make the xs
        %areas over the range be the same
        
        %inputs
        %range- the range to integrate over in eV. will go from E0-range/2
        %to E0+range/2
        function this=initSLBWApprox(this,range)
            area= this.squarePeak*this.squareWidth*this.G;
            X=@(E) 2*(E-this.E0)/this.G;
            down= this.E0-range/2;
            up= this.E0+range/2;
            unitArea=numInt(@(E) 1/(1+X(E)^2),down,up,0.0001);
            this.SLBWPeak=area/unitArea;
        end
        
        function G=getG(this)
            G=this.G;
        end
    end
end