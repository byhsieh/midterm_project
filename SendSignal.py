import numpy as np
import serial
import time

waitTime = 0.1
################################################################
songlength1=42
song1=[261, 261, 392, 392, 440, 440, 392,
      349, 349, 330, 330, 294, 294, 261,
      392, 392, 349, 349, 330, 330, 294,
      392, 392, 349, 349, 330, 330, 294,
      261, 261, 392, 392, 440, 440, 392,
      349, 349, 330, 330, 294, 294, 261]
note1=[1, 1, 1, 1, 1, 1, 2,
      1, 1, 1, 1, 1, 1, 2,
      1, 1, 1, 1, 1, 1, 2,
      1, 1, 1, 1, 1, 1, 2,
      1, 1, 1, 1, 1, 1, 2,
      1, 1, 1, 1, 1, 1, 2]
name1="little star"
speeds1=1
################################################################
songlength2=49
song2=[392,330,330,349
,294,294,261,294,330,349,392,392,392,392,330,330,349
,294,294,261,330,392,392,330,294,294,294,294,294,330,349,330,
330,330,330,330,349,392,392,330,330,349,294,294,261,330,392,392,261]

note2=[1,1,2,1,1,2,1,1,1,1,1,1,2,
       1,1,2,1,1,2,1,1,1,1,4,
       1,1,1,1,1,1,2,1,1,1,1,1,1,2,
       1,1,2,1,1,2,1,1,1,1,4]
name2="little bees"
speeds2=1
################################################################
songlength3=47
song3=[330,330,330,330,330,330,330,392,261,294
,330,349,349,349,349,349,330,330,330,294,294,330,294,
392,330,330,330,330,330,330,330,392,261,294,330,349,349,349
,349,349,330,330,392,392,349,294,261]

note3=[1,1,2,1,1,2,1,1,1,1,4,
        1,1,2,1,1,1,1 ,1,1,1,1,2,2,
       1,1,2,1,1,2,1,1,1,1,4,
        1,1,2,1,1,1,1 ,1,1,1,1,2,2]
name3="jingle bells"
speeds3=1
################################################################
songlength4=32
song4=[261,294,330,261,261,294,330,261,330,349,392,330,349,392,392,440,392,349,330,261,392,440,392,349,330,261,261,196,261,261,196,261]
note4=[2,2,2,2,2,2,2,2,
      2,2,4,2,2,4,
      1,1,1,1,2,2,1,1,1,1,2,2,
      2,2,4,2,2,4]
name4="two tigers"
speeds4=2
################################################################
songlength5=38
song5=[392,392,330,261,392,392,330,261,294,330,349,349,330,349,349,392,392,330,392,330,294,330,261,349,294,294,294,330,261,261,261,294,330,349,294,261,247,261]
note5=[1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,2,
         1,1,1,1,1,1,1,1,
         1,1,1,1,1,1,2]
name5="train run"
speeds5=1

################################################################

################################################################
formatter = lambda x: "%d" % x

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
#print("Sending signal ...")
#print("It may take about %d seconds ..." % (int(signallength * waitTime)))
#for data in note2:
  # print(data)
#print(songlength2)
s.flushOutput()

s.write(bytes("#",'UTF-8'))
s.write(bytes(formatter(songlength1),'UTF-8'))
time.sleep(waitTime)
s.write(bytes(formatter(speeds1),'UTF-8'))
time.sleep(waitTime)
#s.write(bytes('\0','UTF-8'))
time.sleep(waitTime)
s.write(bytes(name1,'UTF-8'))
time.sleep(waitTime)
s.write(bytes("$",'UTF-8'))


for data in song1:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
for data in note1:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
#s.write(bytes("ffffff",'UTF-8'))
#s.close()
#print("Signal sended")

s.write(bytes("#",'UTF-8'))
s.write(bytes(formatter(songlength2),'UTF-8'))
time.sleep(waitTime)
s.write(bytes(formatter(speeds2),'UTF-8'))
time.sleep(waitTime)
#s.write(bytes('\0','UTF-8'))
time.sleep(waitTime)
s.write(bytes(name2,'UTF-8'))
time.sleep(waitTime)
s.write(bytes("$",'UTF-8'))


for data in song2:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
for data in note2:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
#s.write(bytes("ffffff",'UTF-8'))
#s.close()
#print("Signal sended")

s.write(bytes("#",'UTF-8'))
s.write(bytes(formatter(songlength3),'UTF-8'))
time.sleep(waitTime)
s.write(bytes(formatter(speeds3),'UTF-8'))
time.sleep(waitTime)
#s.write(bytes('\0','UTF-8'))
time.sleep(waitTime)
s.write(bytes(name3,'UTF-8'))
time.sleep(waitTime)
s.write(bytes("$",'UTF-8'))


for data in song3:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
for data in note3:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
#s.write(bytes("ffffff",'UTF-8'))
#s.close()
#print("Signal sended")

s.write(bytes("#",'UTF-8'))
s.write(bytes(formatter(songlength4),'UTF-8'))
time.sleep(waitTime)
s.write(bytes(formatter(speeds4),'UTF-8'))
time.sleep(waitTime)
#s.write(bytes('\0','UTF-8'))
time.sleep(waitTime)
s.write(bytes(name4,'UTF-8'))
time.sleep(waitTime)
s.write(bytes("$",'UTF-8'))


for data in song4:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
for data in note4:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
#s.write(bytes("ffffff",'UTF-8'))
#s.close()
#print("Signal sended")

s.write(bytes("#",'UTF-8'))
s.write(bytes(formatter(songlength5),'UTF-8'))
time.sleep(waitTime)
s.write(bytes(formatter(speeds5),'UTF-8'))
time.sleep(waitTime)
#s.write(bytes('\0','UTF-8'))
time.sleep(waitTime)
s.write(bytes(name5,'UTF-8'))
time.sleep(waitTime)
s.write(bytes("$",'UTF-8'))


for data in song5:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
for data in note5:
  s.write(bytes(formatter(data) ,'UTF-8'))
  time.sleep(waitTime)
#s.write(bytes("ffffff",'UTF-8'))
s.close()
#print("Signal sended")
