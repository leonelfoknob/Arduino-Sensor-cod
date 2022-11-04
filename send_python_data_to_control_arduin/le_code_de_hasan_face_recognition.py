from cv2 import*
from numpy import *
import serial
import time

arduino = serial.Serial(port='COM3', baudrate=115200, timeout=.1)

recognizer =cv2.face.LBPHFaceRecognizer_create()
recognizer.read('code/code.yml')
cascadePath="haarcascade_frontalface_alt.xml"
faceCascade=CascadeClassifier(cascadePath);

font=FONT_HERSHEY_SIMPLEX
cam2=VideoCapture(0)



while(True):
    ret,img2=cam2.read()
    gray =cvtColor(img2,COLOR_BGR2GRAY)
    faces = faceCascade.detectMultiScale(gray,1.5,5)

    for(x,y,w,h) in faces:
        rectangle(img2,(x-20,y-20),(x+w+20,y+h+20),(0,255,0),4)
        Id,conf= recognizer.predict(gray[y:y+h,x:x+w])

        if   (Id == 1 and conf<40):
                    Id = "hasan gray"
                    arduino.write(bytes('1', 'utf-8'))# turn green led on
                    print(Id,conf)
        elif (Id == 2 and conf<60):
                    Id = "hasan renkli"
                    arduino.write(bytes('1', 'utf-8'))# turn green led on
                    print(Id)
        elif (Id == 3 and conf<60):
                    Id = "hasan"
                    arduino.write(bytes('1', 'utf-8'))# turn green led on
                    print(Id,conf)
        elif (Id == 4 and conf < 60):
                    Id = "Sevval"
                    arduino.write(bytes('1', 'utf-8'))# turn green led on
                    print(Id, conf)
        else:

                    Id = "Unknown"
                    arduino.write(bytes(0, 'utf-8'))# turn red led on
                    print(Id, conf)



        rectangle(img2, (x - 22, y - 90), (x + w + 22, y - 22), (0,255,0),-1)
        putText(img2, str(Id), (x, y - 40), font, 2, (0,0,0), 3)

    imshow('Yuz Tanima', img2)

    if waitKey(10)& 0xFF == ord('q'):
            break

cam2.release()
destroyAllWindows()