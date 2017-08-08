# MAE-490 LIFTOFF Team 3
# Main data processing script
# Created by: Bradley Henderson
# Date: 7/29/2017
#   Last updated: 8/8/2017


import math
import numpy
# import glob
import matplotlib.pyplot as plt

#import flightData    #may not be used, idk yet

#The following line needs to be changed based on the saving style, the headers
#  must be in order as determined by the Arduino data saving
#colHeaders = ['time_millis','atmoPressureA','atmoPressureB','altA','altB',
#              'intPressure','intTemp','ax','ay','az','mx','my','mz',
#              'roll','pitch','yaw']

colHeaders = ['time_millis','atmoPressureA','atmoPressureB']

#Ask for file name to process
#filename = input('What is the data file to process? : ')   #ask user for data file to process
filename = 'pressureTestData.csv'
print('The filename being processed is:',filename)                         #print what filename was input
flightData = numpy.loadtxt(fname=filename, delimiter=',')  #get the flight data from the file


#function to retrieve data based on what column header is requested
def getData(colHeaders,data_array,variable_interest): 
    for i in range(len(colHeaders)):            #loops over entire 'colHeaders' list
        j = 0
        if colHeaders[i] == variable_interest:  #finds column index for requested data
                j = i                               #hold the index value
    dataRequested = data_array[:,j]                 #column of data
    return dataRequested                            #return the requested data


#get time (in ms)
if 'time_millis' in colHeaders:
    time_ms = getData(colHeaders,flightData,'time')
    time_seconds = time_ms/1000.0
#get atmosheric pressures
if 'atmoPressureA' in colHeaders:
    atmoPressureA = getData(colHeaders,flightData,'atmoPressureA')
    atmoPressureB = getData(colHeaders,flightData,'atmoPressureB')
#get altitudes
if 'altA' in colHeaders:
    altA = getData(colHeaders,flightData,'altA')
    altB = getData(colHeaders,flightData,'altB')
#get internal pressure
if 'intPressure' in colHeaders:
    intPressure = getData(colHeaders,flightData,'intPressure')
#get internal temp.
if 'intTemp' in colHeaders:
    intTemp = getData(colHeaders,flightData,'intTemp')
#get accelerations
if 'ax' in colHeaders:
    ax = getData(colHeaders,flightData,'ax')
    ay = getData(colHeaders,flightData,'ay')
    az = getData(colHeaders,flightData,'az')
#get gyro rates (degrees/sec)
if 'roll' in colHeaders:
    roll  = getData(colHeaders,flightData,'roll')
    pitch = getData(colHeaders,flightData,'pitch')
    yaw   = getData(colHeaders,flightData,'yaw')
#get magnetometer data
if 'mx' in colHeaders:
    mx = getData(colHeaders,flightData,'mx')
    my = getData(colHeaders,flightData,'my')
    mz = getData(colHeaders,flightData,'mz')





##### Plotting section ####
if 'time_millis' in colHeaders:
    if 'atmoPressureA' in colHeaders:
    #atmospheric pressure plot(s)
        fig_pressure = plt.figure(1)
        plt.title('Atmospheric Pressure vs. Time') # title
        
        axes1 = fig_pressure.add_subplot(2, 1, 1)
        axes2 = fig_pressure.add_subplot(2, 1, 2)
        
        axes1.set_ylabel('PressureA [Pa]')
        axes1.set_xlabel('Time [s]')
        axes1.plot(time_seconds,atmoPressureA)
        
        axes2.set_ylabel('PressureB [Pa]')
        axes2.set_xlabel('Time [s]')
        axes2.plot(time_seconds,atmoPressureB)
        
        fig_pressure.tight_layout()
        plt.show()
        plt.savefig('pressurePlot.png', bbox_inches='tight')
        
    if 'altA' in colHeaders:
    #altitude plot(s)
        fig_altitude = plt.figure(figsize=(10.0, 3.0))
        fig_altitude.title('Altitude vs. Time') # title
        
        axes1 = fig_altitude.add_subplot(1, 2, 1)
        axes2 = fig_altitude.add_subplot(1, 2, 2)
        
        axes1.set_ylabel('AltitudeA [feet]')
        axes1.set_xlabel('Time [s]')
        axes1.plot(time_seconds,altA)
        
        axes2.set_ylabel('AltitudeB [feet]')
        axes2.set_xlabel('Time [s]')
        axes2.plot(time_seconds,altB)
        
        fig_altitude.tight_layout()
        plt.show()
        plt.savefig('altitudePlot.png', bbox_inches='tight')
    
    if 'intPressure' in colHeaders:
    #internal pressure plot(s)
        fig_intPressure = plt.figure(time_seconds,intPressure,figsize=(10.0, 3.0))
        fig_intPressure.title('Internal Pressure vs. Time') # title
        axes1.set_ylabel('Pressure [PSI]')
        axes1.set_xlabel('Time [s]')
        
        fig_pressure.tight_layout()
        plt.pyplot.show()
        plt.savefig('internalPressurePlot.png', bbox_inches='tight')        
    
    if 'intTemp' in colHeaders:
    #internal temperature plot(s)
        fig_intTemp = plt.pyplot.figure(time_seconds,intTemp,figsize=(10.0, 3.0))
        fig_intTemp.title('Internal Temperature vs. Time') # title
        axes1.set_ylabel('Temperature [F]')
        axes1.set_xlabel('Time [s]')
        
        fig_pressure.tight_layout()
        plt.pyplot.show()
        plt.savefig('internalTempPlot.png', bbox_inches='tight')
    
    if 'ax' in colHeaders:        
    #acceleration plot(s)
        fig_accel = plt.pyplot.figure(figsize=(10.0, 3.0))
        fig_accel.title('Acceleration vs. Time') # title
        
        axes1 = fig_accel.add_subplot(1, 3, 1)
        axes2 = fig_accel.add_subplot(1, 3, 2)
        axes3 = fig_accel.add_subplot(1, 3, 3)
        
        axes1.set_ylabel('x acceleration [g]')
        axes1.set_xlabel('Time [s]')
        axes1.plot(time_seconds,ax)
        
        axes2.set_ylabel('y acceleration [g]')
        axes2.set_xlabel('Time [s]')
        axes2.plot(time_seconds,ay)
        
        axes3.set_ylabel('z acceleration [g]')
        axes3.set_xlabel('Time [s]')
        axes3.plot(time_seconds,az)
        
        fig_accel.tight_layout()
        plt.pyplot.show()
        plt.savefig('accerlerationPlot.png', bbox_inches='tight')
    
    if 'roll' in colHeaders:
    #angular rates plot(s)
        fig_rates = plt.pyplot.figure(figsize=(10.0, 3.0))
        fig_rates.title('Angular Rates vs. Time') # title
        
        axes1 = fig_rates.add_subplot(1, 3, 1)
        axes2 = fig_rates.add_subplot(1, 3, 2)
        axes3 = fig_rates.add_subplot(1, 3, 3)
        
        axes1.set_ylabel('Roll [deg/s]')
        axes1.set_xlabel('Time [s]')
        axes1.plot(time_seconds,roll)
        
        axes2.set_ylabel('Pitch [deg/s]')
        axes2.set_xlabel('Time [s]')
        axes2.plot(time_seconds,pitch)
        
        axes3.set_ylabel('yaw [deg/s]')
        axes3.set_xlabel('Time [s]')
        axes3.plot(time_seconds,yaw)
        
        fig_accel.tight_layout()
        plt.pyplot.show()
        plt.savefig('angularRatesPlot.png', bbox_inches='tight')
    if 'mx' in colHeaders:
    #magnetometer plot(s)
        fig_mag = plt.pyplot.figure(figsize=(10.0, 3.0))
        fig_mag.title('Angular Rates vs. Time') # title
        
        axes1 = fig_mag.add_subplot(1, 3, 1)
        axes2 = fig_mag.add_subplot(1, 3, 2)
        axes3 = fig_mag.add_subplot(1, 3, 3)
        
        axes1.set_ylabel('x Field [mGauss]')
        axes1.set_xlabel('Time [s]')
        axes1.plot(time_seconds,mx)
        
        axes2.set_ylabel('y Field [mGauss]')
        axes2.set_xlabel('Time [s]')
        axes2.plot(time_seconds,my)
        
        axes3.set_ylabel('z Field [mGauss]')
        axes3.set_xlabel('Time [s]')
        axes3.plot(time_seconds,mz)
        
        fig_mag.tight_layout()
        plt.pyplot.show()
        plt.savefig('magneticFieldPlot.png', bbox_inches='tight')