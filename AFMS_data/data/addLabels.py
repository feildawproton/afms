import os
import csv

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

for pathTuple in paths:
    if(pathTuple[0] != '.'):
        for name in pathTuple[2]:

            label1 = ''
            label2 = ''
            label3 = '' 

            found = False           

            for row in labels:
                
                if(row[0] == name):

                    if(len(row[1]) == 1):
                        label1 = row[1]
                    elif(len(row[1]) == 2):
                        label1 = row[1]
                    else:
                        print('label 1 is not of expected length')

                    if(len(row[2]) == 1):
                        label2 = row[2]
                    elif(len(row[2]) == 2):
                        label2 = row[2]
                    else:
                        print('label 2 is not of expected length')

                    if(len(row[3]) == 1):
                        label3 = row[3]
                    elif(len(row[3]) == 2):
                        label3 = row[3]
                    else:
                        print('label 3 is not of expected length')
            
                    found = True
                    countMatches += 1
                
            if not found:
                    print(name)


            fullPath = os.path.join(pathTuple[0], name)           
            with open(fullPath, 'rb+') as f:

                bytes = f.read()

                if(len(bytes) == unLabeledLength):
                    bytes += label1                    
                    bytes += label2
                    bytes += label3
                elif(len(bytes) == LabeledLength):
                    bytes = (bytes[:(unLabeledLength)])
                    bytes += label1                    
                    bytes += label2
                    bytes += label3
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
                    countOops2 += 1

            
            

print(str(countMatches) + ' matches found')

print(str(countOops) + ' files of unexpected length')
print(str(countNoLabels2) + ' files without label bytes')
print(str(countLabeled2) + ' files WITH label bytes')
print(str(countOops2) + ' labeled mistakes')
