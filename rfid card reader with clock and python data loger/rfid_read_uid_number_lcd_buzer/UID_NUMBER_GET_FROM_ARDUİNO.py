import serial
import time

ser = serial.Serial('COM4',115200, timeout=0.1)

#arduino = serial.Serial('COM3',115200, timeout=0.1)


def read_sensor_data():
    data = ser.readline()
    return data.decode('utf-8')
"""
while True:
    Myfile = open('uid_number.txt','a+')
    value = read_sensor_data()
    if value:#verifi si la ligne n'est pas vide si c'est vide sa ne fait rien au cas contraire sa enregistre et sa break
        Myfile.write('\n'+'uid:'+ str(value))
        print(value)
    Myfile.close()
"""
while True:
    value = read_sensor_data()
    Myfile = open('uid_number.txt','a+')
    if value:#verifi si la ligne n'est pas vide si c'est vide sa ne fait rien au cas contraire sa enregistre et sa break
        Myfile.write('\n'+'uid:'+ value)        
        #print(value)
        break
    Myfile.close()

