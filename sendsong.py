import numpy as np
import serial
import time

waitTime = 0.1
############################## SONG1 INFO ##############################
name1="Canon"

song1=[659,587,523,494,
     440,392,440,494,
     659,659,523,587,494,523,523,440,494,392,
     440,440,349,392,330,440,440,523,494,523,587,
     659,587,659,698,784,587,784,698,659,880,784,698,784,698,659,587,
     523,440,880,988,1046,988,880,784,698,659,587,880,784,880,784,698]

note1=[4,4,4,4,
     4,4,4,4,
     2,1,1,3,1,2,1,1,3,1,
     2,1,1,3,1,2,1,1,2,1,1,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]

songlength1=61
speeds1=1
########################################################################

############################## SONG2 INFO ##############################
name2="Cuckoo"

song2=[523,440,523,440,392,349,392,349,
     392,392,440,494,392,440,440,523,523,440,
     523,440,523,440,494,440,392,349]

note2=[1,2,1,2,1,1,1,2,
     1,1,1,2,1,1,1,1,2,1,
     1,2,1,2,1,1,1,2]

songlength2=26
speeds2=1
########################################################################

############################## SONG3 INFO ##############################
name3="Snail and bird"

song3=[392,392,0,392,392,0,330,392,262,440,392,
     392,392,0,392,392,0,330,294,262,330,294,
     294,330,392,392,330,330,294,262,262,
     294,330,262,220,175,220,175]

note3=[1,1,1,1,1,1,1,1,2,2,4,
     1,1,1,1,1,1,1,1,2,2,4,
     2,2,2,2,2,1,1,2,2,
     2,2,2,2,2,2,4]

songlength3=38
speeds3=2
########################################################################

############################## SONG4 INFO ##############################
name4="Happy Birthday"

song4=[262,262,294,262,349,330,
     262,262,294,262,392,349,
     262,262,523,440,349,330,294,
     494,494,440,349,392,349]

note4=[1,1,1,1,1,2,
     1,1,1,1,1,2,
     1,1,1,1,1,1,2,
     1,1,1,1,1,2]

songlength4=25
speeds4=1
########################################################################

############################## SONG5 INFO ##############################
name5="Ode to Joy"

song5=[330,330,349,392,392,349,330,294,
    262,262,294,330,330,294,294,
    330,330,349,392,392,349,330,294,
    262,262,294,330,294,262,262,
    294,294,330,262,294,330,349,330,262,
    294,330,349,330,294,262,294,262,
    330,330,349,392,392,349,330,294,
    262,262,294,330,294,262,262]

note5=[1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,2,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,2,
    1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,2,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,2]

songlength5=62
speeds5=1
########################################################################

############################## SONG6 INFO ##############################
name6="Jingle Bell"

song6=[330,330,330,330,330,330,330,392,261,294,330,
    349,349,349,349,349,330,330,330,294,294,330,294,392,
    330,330,330,330,330,330,330,392,261,294,330,
    349,349,349,349,349,330,330,392,392,349,294,261]

note6=[1,1,2,1,1,2,1,1,1,1,4,
    1,1,2,1,1,1,1 ,1,1,1,1,2,2,
    1,1,2,1,1,2,1,1,1,1,4,
    1,1,2,1,1,1,1 ,1,1,1,2,2]

songlength6=47
speeds6=1
########################################################################

############################## SONG7 INFO ##############################
name7="Little Bee"

song7=[392,330,330,349,
    294,294,261,294,330,349,392,392,392,392,330,330,349,
    294,294,261,330,392,392,330,294,294,294,294,294,330,349,330,
    330,330,330,330,349,392,392,330,330,349,294,294,261,330,392,392,261]

note7=[1,1,2,1,1,2,1,1,1,1,1,1,2,
    1,1,2,1,1,2,1,1,1,1,4,
    1,1,1,1,1,1,2,1,1,1,1,1,1,2,
    1,1,2,1,1,2,1,1,1,1,4]

songlength7=49
speeds7=1
########################################################################

############################## SONG8 INFO ##############################
name8="Little Star"

song8=[261, 261, 392, 392, 440, 440, 392,
       349, 349, 330, 330, 294, 294, 261,
       392, 392, 349, 349, 330, 330, 294,
       392, 392, 349, 349, 330, 330, 294,
       261, 261, 392, 392, 440, 440, 392,
       349, 349, 330, 330, 294, 294, 261]

note8=[1, 1, 1, 1, 1, 1, 2,
       1, 1, 1, 1, 1, 1, 2,
       1, 1, 1, 1, 1, 1, 2,
       1, 1, 1, 1, 1, 1, 2,
       1, 1, 1, 1, 1, 1, 2,
       1, 1, 1, 1, 1, 1, 2]

songlength8=42
speeds8=1
########################################################################

########################################################################
formatter = lambda x: "%d" % x

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

song = song1 + note1 + song2 + note2 + song3 + note3 + song4 + note4 + song5 + note5 + song6 + note6 + song7 + note7 + song8 + note8 
#songlength = 2 * (songlength1 + songlength2 + songlength3 + songlength4 + songlength5)

#songs = []
#for num in song:
#       songs.append(float(num/1000))

#songtable = np.array(song)

print("Start sending songs ...")
print("It may take about %d seconds ..." % (int(len(song) * waitTime)))
for data in song:
  s.write(bytes(formatter(data), 'UTF-8'))
  time.sleep(waitTime)

s.close()
print("Songs are sended completely !!")
