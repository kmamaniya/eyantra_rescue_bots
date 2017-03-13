 '''
     Project Name:     e-Yantra Project
     Author List:      Joshua Koyeerath, Shaun Kollannur 
     Filename:         XBee_Receive_v6.py
     
     Functions:        initialize_var(),
                       open_serialComm(port),
                       output_image(x,y,z,htmlfile),
                       createPlan(plot_fname,z_plan),
                       deleteContent(pfile),
                       fillXYZ(filename),
                       moveBot(frecord_clean),
                       scanArena(fplot_clean),
                       shiftOfOrigin(lines_accessed,theta_temp,x_temp,y_temp,input_file),
                       targetVictim(fname),
                       retracePath(fname),
                       moveRescueBot(fname)
                       
     Global Variables: x_dist,
                       y_dist,
                       new_theta,
                       fplot_clean,
                       frecord_clean

 '''

import plotly.plotly as py
import plotly.offline as of
import plotly.graph_objs as go
import math
import serial
import time
from plotly import tools
from xbee import XBee

x_dist=0;
y_dist=0;
new_theta=0;
fplot_clean = 0;
frecord_clean = 0;

'''
    Function Name:    initialize_var
    Input:            NONE
    Output:           r, x, y, z, theta, phi
    Logic:            Initialize the lists (Radius, X, Y, Z, theta, phi)
                      The list are initialized with size zero in order to append the results
                      after each scan
    Example call:     initialize_var()
'''

def initialize_var():
    # Initialize the array holding the incoming points
    r = range(0)
    # Intialize arrays for x, y and z respectively
    x = range(0)
    y = range(0)   
    z = range(0)
    theta = range(0)
    phi = range(0)

    return r,x,y,z,theta,phi

'''
    Function Name:    open_serialComm
    Input:            port (The port to open)
    Output:           serial_port
    Logic:            Enable USB port for XBee communication
    Example call:     open_serialComm(port)
'''

def open_serialComm(port):
    # Enable USB communication
    serial_port = serial.Serial(port,9600)
    #xbee = XBee(serial_port)

    # Refresh the port to be used
    serial_port.close()
    serial_port.open()

    print 'Serial communication established !!'
    return serial_port

'''
    Function Name:    output_image
    Input:            x, y, z, htmlfile
    Output:           Graphical Output 
    Logic:            Output plots using the plot.ly python library
    Example call:     output_image(x,y,z,htmlfile)
'''

def output_image(x,y,z,htmlfile):
    trace=go.Scatter3d(
                            # set color to an array/list of desired values
                            # set color to an array/list of desired values

    x= x,y=y,z=z,mode='markers',marker=dict(
            color='rgb(91, 92, 19)',
            size=4,
            line=dict(
                color='rgb(204, 204, 204)',
                width=2
            ),
            opacity=0.7
        ))
    data=[trace]
    of.plot(data, filename=htmlfile)

'''
    Function Name:    createPlan
    Input:            plot_fname, z_plan (Both filenames are passed)
    Output:           zplot_file (Write on this file)
    Logic:            From the obtained points through the scanning, create a top-view plan of the arena
    Example call:     createPlan(plot_fname,z_plan)
'''

def createPlan(plot_fname,z_plan):
    plot_file = open(plot_fname,'r')
    zplot_file = open(z_plan,'w')

    for line in plot_file: 
        line = line.split('\n')
        r2temp = line[0].split(',')
        ztemp = float(r2temp[2])

        if(ztemp < 175 and ztemp > 0):
            zplot_file.writelines(str(r2temp[0])+','+str(r2temp[1])+',0')
            zplot_file.writelines('\n')

    print 'Filtering Complete !!'
    plot_file.close()
    zplot_file.close()
    
'''
    Function Name:    deleteContent
    Input:            pfile (Filename)
    Output:           pfile (The file data is erased)
    Logic:            The file data is completely cleared
    Example call:     deleteContent(pfile)
'''

def deleteContent(pfile):
    pfile.seek(0)
    pfile.truncate()

'''
    Function Name:    fillXYZ
    Input:            filename
    Output:           x, y, z arrays
    Logic:            The file data is read and the x, y and z values are returned
    Example call:     fillXYZ(filename)
'''

def fillXYZ(filename):
    x = range(0)
    y = range(0)
    z = range(0)
    file = open(filename,'r')

    for line in file:
        line = line.split('\n')
        rtemp = line[0].split(',')
        x.extend([rtemp[0]])
        y.extend([rtemp[1]])
        z.extend([rtemp[2]])
    return x,y,z

'''
    Function Name:    moveBot
    Input:            frecord_clean
    Output:           Store path moved in PathRecord.txt, frecord_clean
    Logic:            Send information about path to bot, also store path moved in PathRecord.txt
    Example call:     moveBot(frecord_clean)
'''

def moveBot(frecord_clean):
    # Start sending instructions..
    path_file = open('PathRecord.txt','a+')

    # If the file is accessed for the very first time,(i.e. frecord_clean = 0)
    if(frecord_clean == 0):
        deleteContent(path_file)

    frecord_clean += 1

    i = 0     
    while(1):
        outgoing = raw_input('\nEnter number to transmit : ')
        serial_port.write(outgoing)
        if(outgoing == ";"):
            print '\nEnd char received. Terminating...'
            path_file.write('\n')
            break
        if(outgoing == ","):
            i += 1
            if(i == 1):
                path_file.write(outgoing)
                continue
            if(i == 2):
                path_file.write('\n')
                i = 0
                continue
        path_file.write(outgoing)
        print 'Written in file'
            
    path_file.close()
    
    return frecord_clean

'''
    Function Name:    scanArena
    Input:            fplot_clean
    Output:           fplot_clean
    Logic:            Receives scaned information from bot and then stores the information
                      in a particular format in the file(3DPlot)
    Example call:     scanArena(fplot_clean)
'''

def scanArena(fplot_clean):
    plot_file = open('3DPlot.txt','a+')
    r,x,y,z,theta,phi = initialize_var()

    # If the file is accessed for the very first time,(i.e. fplot_clean = 0)
    if(fplot_clean == 0):
        deleteContent(plot_file)

    fplot_clean += 1

    print 'Waiting for inputs.....\n'
        
    while(1):
        
        r.extend([60 + (int(serial_port.read().encode('hex'),16) * 256 + int(serial_port.read().encode('hex'),16))])
        theta.extend([int(serial_port.read().encode('hex'),16)])
        phi.extend([int(serial_port.read().encode('hex'),16)])
        
        print 'Phi : ',phi[-1],'\t'
        
        if(theta[-1] == 201 and phi[-1] == 201):
            # Indicates end of scanning
            break
        if(r[-1] > 1500):
            continue
            
        # Convert polar co-ordinates to cartesian and fill in respective arrays
        print 'Theta : ',theta[-1],' R : ',r[-1],'\t'
        x.extend([x_dist + r[-1] * math.cos(math.radians((360 - new_theta)%360 + theta[-1])) * math.sin(math.radians(140-phi[-1]))])
        y.extend([y_dist + r[-1] * math.sin(math.radians((360 - new_theta)%360 + theta[-1])) * math.sin(math.radians(140-phi[-1]))])
        z.extend([155 + (r[-1] * math.cos(math.radians(140-phi[-1])))])

        if(z[-1] >= 1200):
            x.pop()
            y.pop()
            z.pop()
            continue

        plot_file.writelines(str(x[-1])+','+str(y[-1])+','+str(z[-1])+'\n')

    print 'Phi : ',phi[-1],'\t'
    print 'Input taking completed'
    # Close the file
    plot_file.close()

    x,y,z = fillXYZ('3DPlot.txt')
    
    print 'Output the received points'
    output_image(x,y,z,'basic-scatter.html')

    print 'Creating a plan of the arena'
    createPlan('3DPlot.txt','ZPlot.txt')

    print 'Filling the x,y and z arrays with contents of the plan file'
    x,y,z = fillXYZ('ZPlot.txt')

    print 'Output the Z plan'
    output_image(x,y,z,'Z_Plan.html')

    return fplot_clean

'''
    Function Name:    shiftOfOrigin
    Input:            lines_accessed, theta_temp, x_temp, y_temp, input_file
    Output:           lines_accessed, theta_temp,x_temp,y_temp
    Logic:            Performs shift of origin after the robot moves from initial position
    Example call:     shiftOfOrigin(lines_accessed, theta_temp,x_temp,y_temp,input_file)
'''

def shiftOfOrigin(lines_accessed,theta_temp,x_temp,y_temp,input_file):
    path_file = open(input_file,'r');
    t_dist=range(0);
    t_theta=range(0);
    c=0;
    for line in path_file:
        temp=line.split('\n');
        temp=temp[0].split(',');
        t_dist.extend([int(temp[0])]);
        t_theta.extend([90 * int(temp[1])]);
        c+=1;
    for i in range(c):
        if(i <= lines_accessed):
            # These lines have already been accessed
            continue
        theta_temp = (theta_temp+t_theta[i])%360;
        x_temp += t_dist[i]*math.sin(math.radians(theta_temp));
        y_temp += t_dist[i]*math.cos(math.radians(theta_temp));
        lines_accessed += 1

    print lines_accessed,theta_temp,x_temp,y_temp

    return lines_accessed,theta_temp,x_temp,y_temp

'''
    Function Name:    targetVictim
    Input:            fname (The name of the file to write on)
    Output:           Stores the victim location in the file
    Logic:            Co-ordinates of the victim are accepted and the Analyser bot comes back using the retracePath() functtion
    Example call:     targetVictim(fname)
'''

def targetVictim(fname):
    path_file = open(fname,'a+')
    
    i = 0     
    while(1):
        outgoing = raw_input('\nEnter number to transmit : ')
        if(outgoing == ";"):
            print '\nEnd char received. Terminating...'
            break
        if(outgoing == ","):
            i += 1
            if(i == 1):
                path_file.write(outgoing)
                continue
            if(i == 2):
                path_file.write('\n')
                i = 0
                continue
        path_file.write(outgoing)
    print 'File writing completed !!'
    path_file.close()

    retracePath(fname)

'''
    Function Name:    retracePath
    Input:            fname (The name of the file to read from)
    Output:           The Analyser bot comes to it's original position
    Logic:            The bot reads the record file in reverse to come back to it's original position
    Example call:     retracePath(fname)
'''

def retracePath(fname):
    path_file = open(fname,'r')

    data = path_file.read().split('\n')
    di = -2
    try:
        while(1):
            xyz = data[di]
            print xyz
            i =  0
            while(i < len(xyz)):
                time.sleep(1)
                serial_port.write(str(xyz[i]))
                i += 1
            time.sleep(1)
            serial_port.write(';')
            di -= 1
            print 'Waiting to receive retrace acknowledgement...'
            temp = serial_port.read()
    except IndexError:
        print 'Trace File reading complete !!'

'''
    Function Name:    moveRescueBot
    Input:            fname (The name of the file to read from)
    Output:           The Rescue bot has reached the position were the victim is present
    Logic:            The Rescue bot reads the record file to reach to the victim
    Example call:     moveRescueBot(fname)
'''    

def moveRescueBot(fname):
    path_file = open(fname,'r')

    data = path_file.read()
    i = 0
    time.sleep(1)
    while(i < len(data)):
        time.sleep(1)
        if(data[i] == '\n'):
            serial_port.write(',')
            i += 1
            continue
        print '\nData[',i,'] : ',data[i]
        serial_port.write(str(data[i]))
        i += 1
    serial_port.write(';')

    print 'Inputs to move given !!'

    path_file.close()


# Begin Execution
# Working with analyzer bot
print '\nEstablishing communication with Analyzer Bot....'
serial_port = open_serialComm('COM12')


input2 = raw_input('Do you want to initiate ?')
while(input2 != 'y'):
    input2 = raw_input('Do you want to initiate ?')

print '\nInitiate the communication...'

lines_accessed = 0

while(1):
    print '\n------- Analyze Options -------\n1 : Scan\n2 : Move\n3 : Target victim'
    outgoing = raw_input('\nEnter option : ')
    if(outgoing == '1'):
        serial_port.write("1")
        fplot_clean = scanArena(fplot_clean)
    elif(outgoing == '2'):
        serial_port.write("2")
        frecord_clean = moveBot(frecord_clean)
        lines_accessed,new_theta,x_dist,y_dist = shiftOfOrigin(lines_accessed,new_theta,x_dist,y_dist,'PathRecord.txt')
    else:
        serial_port.write('3')
        targetVictim('PathRecord.txt')
        break


# Close the port
serial_port.close()



# Working with the rescue bot
print '\nEstablishing communication with Rescue Bot....'
serial_port = open_serialComm('COM6')

while(1):
    print '\n------- Rescue Options -------\n1 : Move\n2 : Grab\n3 : Return Home\n4 : Release'
    outgoing = raw_input('\nEnter option : ')
    if(outgoing == '1'):
        serial_port.write(outgoing)
        moveRescueBot('PathRecord.txt')
    else:
        serial_port.write(outgoing)
    if(outgoing == '3'):
        print '\nRescue bot returning home...'
        break

print '\nOperation terminated.'
