# MAE-490 LIFTOFF Team 3
# Main data processing script
# Created by: Bradley Henderson
# Date: 7/29/2017


import math
import numpy
import glob
import matplotlib.pyplot

#import flightData    #may not be used, idk yet

#The following line needs to be changed based on the saving style
colHeaders = ['time_millis','atmoPressureA','atmoPressureB','intPressure','intTemp','ax','ay','az',]
#Ask for file name to process
filename = String(input('What is the data file to process? : ')) #ask user for data file to process
flightData = numpy.loadtxt(fname=filename, delimiter=',')  #get the flight data from the file
print('The filename is:',filename)                   #print what filename was input



#get time (in ms)
time = getData(colHeaders,flightData,'time')
#get atmos. pressures
atmoPressureA = getData(colHeaders,flightData,'atmoPressureA')
#get altitudes

#get internal pressure

#get internal temp.

#get accelerations
ax = flightData.xg
ay = flightData.xy
az = flightData.xz

#getgyro rates (degrees/sec)
roll  = getData(colHeaders,flightData,'roll')
pitch = getData(colHeaders,flightData,'pitch')
yaw   = getData(colHeaders,flightData,'yaw')
#get magnetometer data



#function to retrieve data based on what column header is requested
def getData(colHeaders,data_array,variable_interest): 
    for i in range(len(colHeaders)):            #loops over entire 'colHeaders' list
        if colHeaders[i] == variable_interest:  #finds column index for requested data
                j = i                               #hold the index value
    dataRequested = data_array[:,j]                 #column of data
    return dataRequested                            #return the requested data
 




##### Plotting section ####

fig = matplotlib.pyplot.figure(figsize=(10.0, 3.0))

axes1 = fig.add_subplot(1, 3, 1)
axes2 = fig.add_subplot(1, 3, 2)
axes3 = fig.add_subplot(1, 3, 3)

axes1.set_ylabel('average')
axes1.plot(numpy.mean(composite_data, axis=0))

axes2.set_ylabel('max')
axes2.plot(numpy.max(composite_data, axis=0))

axes3.set_ylabel('min')
axes3.plot(numpy.min(composite_data, axis=0))

fig.tight_layout()

matplotlib.pyplot.show()
