
# coding: utf-8



get_ipython().magic('matplotlib inline')
import matplotlib.pyplot as plt
import openmc

import os
import numpy as np




#Initial starting source, uniform in fuel only
bounds = [-0.4, -0.4, -0.4, 0.4, 0.4, 0.4]
uniform_dist = openmc.stats.Box(bounds[:3], bounds[3:], only_fissionable=True)




batches=100
settings = openmc.Settings()
settings.source = openmc.source.Source(space=uniform_dist)
settings.batches = batches
settings.inactive = 25  # keep this at a min of 25, this is the number of power iterations performed, more is always better
#settings.particles = 10000  # increase this number to make your results more accurate
settings.particles=100
settings.temperature = {'tolerance':10000,'multipole':True}
#settings.temperature = {'method':'interpolation','multipole':True}
settings.export_to_xml()

def buildRange(lower, upper, steps):
    out=[]
    stepSize=(upper-lower)/(steps)
    buffer=lower
    while buffer<=upper:
        out.append(buffer)
        buffer+=stepSize
    return out





def getU(T):
    #got to watch out for those Uranium diggers
    #nukeLife
    uo2 = openmc.Material(1,"fuel",temperature=T)
    uo2.add_element('U', 1.0, enrichment=4.0)
    uo2.add_element('O', 2.0)
    uo2.set_density('g/cc', 10.0)
    return uo2
def getZirc():
    zirconium = openmc.Material(2, "zirconium", temperature=temp2)
    zirconium.add_element('Zr', 1.0)
    zirconium.set_density('g/cm3', 6.6)
    return zirconium

def getMod(mod, boron,T):
    if(mod=="H2O"):
            #this function creates borated light water, for other moderators you will need to replace
        mod = openmc.model.borated_water(boron_ppm=boron, temperature=T,pressure=15)
    elif(mod=="D2O"):
        mod=openmc.Material(3,"heavy_water",temperature=T)
        mod.add_nuclide('H2',2.0) #add D2
        mod.add_element('O',1.0) #add 0
        mod.set_density('g/cc',0.725) 
        mod.add_s_alpha_beta('c_D_in_D2O')
        #https://webbook.nist.gov/cgi/fluid.cgi?P=15.5&TLow=600&THigh=601&TInc=0.5&Applet=on&Digits=5&ID=C7789200&Action=Load&Type=IsoBar&TUnit=K&PUnit=MPa&DUnit=kg%2Fm3&HUnit=kJ%2Fmol&WUnit=m%2Fs&VisUnit=uPa*s&STUnit=N%2Fm&RefState=DEF
    elif(mod=="GRPHT"):
        pass
    return mod






def buildTallies(cell_filter):
    ###################################Tallies#######################
    #tallies over 2 energy groups with 4 eV being thermal bound
    energy_filter = openmc.EnergyFilter([0., 4.0, 20.0e6])
    t = openmc.Tally(1)
    t.filters = [cell_filter, energy_filter]
    # these are the main reaction rates you should need
    t.scores = ['absorption','nu-fission','fission']
    tallies = openmc.Tallies([t])
    tallies.export_to_xml()
    
    
def parseK(batch):
    #read in the tallies
    sp = openmc.StatePoint('statepoint.'+str(batch)+'.h5')
    # this reads the tally with all reaction rates, not just absorption
    tally = sp.get_tally(scores=['absorption'])
    # this is the final k-effective of the simulation
    os.remove('statepoint.'+str(batch)+'.h5')
    os.remove('summary.h5')
    sp._f.close()
    return sp.k_combined[0]

def parse4Factor(batch,fuel,mod):
        #read in the tallies
    sp = openmc.StatePoint('statepoint.'+str(batch)+'.h5')
    
    uniFilter = openmc.CellFilter([fuel, mod])
    fuelFilter=openmc.CellFilter([fuel])
    # this reads the tally with all reaction rates, not just absorption
    tally = sp.get_tally(scores=['absorption'])
    data=tally.get_pandas_dataframe()
    print(data)
    # this is the final k-effective of the simulation
    
    os.remove('statepoint.'+str(batch)+'.h5')
    os.remove('summary.h5')




def buildRectangUni(modSetting, pitch, diameter,boron,modT,fuelT):
    uo2=getU(fuelT)
    mod=getMod(modSetting,boron,modT) #loads the moderator
    mats = openmc.Materials([uo2, mod])
    mats.export_to_xml()
       
    ####################build the surfaces################################
    fuel_or = openmc.ZCylinder(R=diameter/2)
    box = openmc.get_rectangular_prism(width=pitch, height=pitch,boundary_type='reflective')
        
    #################Build the Cells #######################################
    fuel_region = -fuel_or   #beautiful abuse of operator overloading
    mod_region = box & +fuel_or
    # c
    # c Fuel 
    # c
    # c throwback to MCNP
    fuel = openmc.Cell(1, 'fuel')
    fuel.fill = uo2
    fuel.region = fuel_region
    #
    # Moderator 
    #
    moderator = openmc.Cell(4, 'moderator')
    moderator.fill = mod
    moderator.region = mod_region
       
    #I am groot!
    groot = openmc.Universe(cells=(fuel, moderator))
    geom = openmc.Geometry(groot)
    geom.export_to_xml()
    cell_filter = openmc.CellFilter([fuel, moderator])
    # tallies
    buildTallies(cell_filter)
    return (fuel,moderator)




def Q1a():
    pitch = buildRange(2,25,6)
    boron = [0]
    nt = len(pitch)
    nb = len(boron)
    modT = 900  #fuel temp
    fuelT = 600  #mod temp
    modSetting="D2O"
    diameter=1.0
    k = np.zeros([nt,nb])
    for m in range(nb):
        for j in range(nt):
            #build the problem first
            buildRectangUni(modSetting,pitch[j],diameter,boron[m],modT,fuelT)
            print ("Pitch: "+str(pitch[j])+" Boron: "+str(boron[m]))
            openmc.run(output=False)
            k[j,m] = parseK(batches)
    return (pitch,k)

def Q1b(): #calculates the 4 factor formula for the thingy
    pitch = [13]
    boron = [0]
    nt = len(pitch)
    nb = len(boron)
    modT = 900  #fuel temp
    fuelT = 600  #mod temp
    modSetting="D2O"
    diameter=1.0
    k = np.zeros([nt,4])
    for m in range(nb):
        for j in range(nt):
            #build the problem first
            (fuel,mod)= buildRectangUni(modSetting,pitch[j],diameter,boron[m],modT,fuelT)
            print ("Pitch: "+str(pitch[j])+" Boron: "+str(boron[m]))
            openmc.run(output=False)
            k[j,m] = parse4Factor(batches,fuel,mod)


def plotK(pitch,k,fileName):
    plt.figure(figsize=(20,10))
    plt.plot(pitch,k[:,0], linewidth=10)
    plt.legend(['0ppm Boron'], fontsize=30)
    plt.xlabel('Pitch', fontsize=30)
    plt.ylabel('k', fontsize=30)
    plt.xticks(fontsize=24)
    plt.yticks(fontsize=24)
    plt.savefig(fileName+'.pdf', bbox_inches='tight')




def main():
    #os.remove("*.h5")
    #result=Q1a()
    #plotK(result[0],result[1],'Q1acurve')
    Q1b()

main()
