import os
import csv
import struct

labels = []
with open('labels.csv', 'rb') as f:
    reader = csv.reader(f)
    for row in reader:
        labels.append(row)

paths = os.walk('.')

unLabeledLength = 16384
#LabeledLength = 16396
LabeledLength = 16387

countOops = 0
countNoLabels2 = 0
countLabeled2 = 0
countOops2 = 0

countMatches = 0

#expecting an end lenth of 16390 with 12 label bytes (each label take 4 bytes because of padding or whatever)

bytesTolabel = [b'\x00', b'\x01', b'\x02', b'\x03', b'\x04', b'\x05', b'\x06', b'\x07', b'\x08']

for pathTuple in paths:
    if(pathTuple[0] != '.'):
        for name in pathTuple[2]:

            label1 = 0
            label2 = 0
            label3 = 0

            found = False           

            for row in labels:
                if(row[0] == name):

                    label1 = int(row[1])  
                    label2 = int(row[2])  
                    label3 = int(row[3])  
            
                    found = True
                    countMatches += 1
                
            if not found:
                    print(name)



            fullPath = os.path.join(pathTuple[0], name)           
            with open(fullPath, 'rb+') as f:

                bytes = f.read()

                if(len(bytes) == unLabeledLength):
                    bytes += bytesTolabel[label1]  #be for unsigned char                
                    bytes += bytesTolabel[label2]
                    bytes += bytesTolabel[label3]
                elif(len(bytes) == LabeledLength):
                    bytes = (bytes[:(unLabeledLength)])
                    bytes += bytesTolabel[label1]  #be for unsigned char                
                    bytes += bytesTolabel[label2]
                    bytes += bytesTolabel[label3]
                else:
                    countOops += 1

                f.seek(0)
                f.write(bytes)
                f.truncate()                

                if(len(bytes) == unLabeledLength):
                    countNoLabels2 += 1
                elif(len(bytes) == LabeledLength):
                    countLabeled2 += 1
                else:
                    print(len(bytes))
                    countOops2 += 1

            
            

print(str(countMatches) + ' matches found')

print(str(countOops) + ' files of unexpected length')
print(str(countNoLabels2) + ' files without label bytes')
print(str(countLabeled2) + ' files WITH label bytes')
print(str(countOops2) + ' labeled mistakes')
