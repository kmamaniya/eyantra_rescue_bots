 '''
     Project Name:     e-Yantra Project
     Author List:      Shaun Kollannur, Joshua Koyeerath
     Filename:         XBee_Receive_v5.py
     Functions:        initialize_var(), open_serialComm(), output_image(x,y,z,htmlfile),
                       createPlan(plot_fname,z_plan), deleteContent(pfile), fillXYZ(filename),
                       moveBot(), scanArena(fcounter_check), shiftOfOrigin(theta_temp,x_temp,y_temp,input_file)
     Global Variables: x_dist, y_dist, new_theta, fcounter_check

 '''
# This code works on variable angles and uses file for storage
import plotly.plotly as py
import plotly.offline as of
import plotly.graph_objs as go
import math
import serial
from plotly import tools
from xbee import XBee

x_dist=0;
y_dist=0;
new_theta=0;
fcounter_check = 0;

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
    Input:            NONE
    Output:           serial_port
    Logic:            Enable USB port for XBee communication
    Example call:     open_serialComm()
'''
def open_serialComm():
    # Enable USB communication
    serial_port = serial.Serial('COM12',9600)    # For XBee_B
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
    Logic:            zplot_file 
    Example call:     createPlan(plot_fname,z_plan)
'''
def createPlan(plot_fname,z_plan):
    plot_file = open(plot_fname,'r')
    zplot_file = open(z_plan,'w')

    ##plot_file.seek(0,0)

    for line in plot_file:
        #print line 
        line = line.split('\n')
        #print line
        r2temp = line[0].split(',')
        #print r2temp
        ztemp = float(r2temp[2])

        #Plot points satisfying 0 < z < 100
        if(ztemp < 100 and ztemp > 0):
            zplot_file.writelines(str(r2temp[0])+','+str(r2temp[1])+',0')
            zplot_file.writelines('\n')
            print 'Input in z file\n'

    print 'Filtering Complete !!'

    #Closing the files after read and write operations are over
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
    # Initializing x, y and z as empty list
    x = range(0)
    y = range(0)
    z = range(0)
    # Open file
    file = open(filename,'r')
    #Read line by line and get required information
    for line in file:
        line = line.split('\n')     # Splitting line by using \n
        rtemp = line[0].split(',')  # Splitting x, y and z values using ,
        x.extend([rtemp[0]])
        y.extend([rtemp[1]])
        z.extend([rtemp[2]])
    return x,y,z

'''
    Function Name:    moveBot
    Input:            NONE
    Output:           Store path moved in PathRecord.txt
    Logic:            Send information about path to bot, also store path moved in PathRecord.txt
    Example call:     moveBot()
'''
def moveBot():
    # Start sending instructions..
    path_file = open('PathRecord.txt','w+')

    i = 0     
    while(1):
        outgoing = raw_input('\nEnter number to transmit : ')
        serial_port.write(outgoing)
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
        print 'Written in file'
            
    path_file.close()

'''
    Function Name:    scanArena
    Input:            fcounter_check
    Output:           fcounter_check
    Logic:            Receives scaned information from bot and then stores the information
                      in a particular format in the file(3DPlot)
    Example call:     scanArena(fcounter_check)
'''
def scanArena(fcounter_check):
    plot_file = open('3DPlot.txt','a+')
    r,x,y,z,theta,phi = initialize_var()


    if(fcounter_check == 0):
        deleteContent(plot_file)

    fcounter_check += 1

    print 'Waiting for inputs.....\n'
        
    while(1):

        # To enable midway breaking of program through KeyboardInterrupt
        #temp = raw_input()
        
        r.extend([60 + (int(serial_port.read().encode('hex'),16) * 256 + int(serial_port.read().encode('hex'),16))])
        theta.extend([int(serial_port.read().encode('hex'),16)])
        phi.extend([int(serial_port.read().encode('hex'),16)])
        
        print 'Phi : ',phi[-1],'\t'
        
        if(theta[-1] == 201 and phi[-1] == 201):
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

    return fcounter_check 

'''
    Function Name:    shiftOfOrigin
    Input:            theta_temp, x_temp, y_temp, input_file
    Output:           theta_temp,x_temp,y_temp
    Logic:            Performs shift of origin after the robot moves from initial position
    Example call:     shiftOfOrigin(theta_temp,x_temp,y_temp,input_file)
'''
def shiftOfOrigin(theta_temp,x_temp,y_temp,input_file):
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
        theta_temp = (theta_temp+t_theta[i])%360;
        x_temp += t_dist[i]*math.sin(math.radians(theta_temp));
        y_temp += t_dist[i]*math.cos(math.radians(theta_temp));

    print theta_temp,x_temp,y_temp

    return theta_temp,x_temp,y_temp


# Begin Execution

serial_port = open_serialComm()


input2 = raw_input('Do you want to initiate ?')
while(input2 != 'y'):
    input2 = raw_input('Do you want to initiate ?')

print '\nInitiate the communication...'
# Menu to control robot
while(1):
    print '\n------- Options -------\n1 : Scan\n2 : Move'
    if(raw_input('\nEnter option : ') == '1'):
        serial_port.write("1")
        fcounter_check = scanArena(fcounter_check)
    else:
        serial_port.write("2")
        moveBot()
        new_theta,x_dist,y_dist = shiftOfOrigin(new_theta,x_dist,y_dist,'PathRecord.txt')
    


# Move the bot to it's next location
moveBot()

# Close the port
serial_port.close()
