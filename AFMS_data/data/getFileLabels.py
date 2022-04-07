import os
import csv

paths = os.walk('.')

LabeledLength = 16396
LabeledLength = 16387
#expecting an end lenth of 16390 with 12 label bytes (each label take 4 bytes because of padding or whatever)

for pathTuple in paths:
    if(pathTuple[0] != '.'):
        for name in pathTuple[2]:

            fullPath = os.path.join(pathTuple[0], name)           
            with open(fullPath, 'rb') as f:

                bytes = f.read()
                
                #print(len(bytes))
                if(len(bytes) != LabeledLength):
                    print("file " + str(name) + " does not have expected length")

                #this indexing indexes weird.  by length I guess?
                #label3 = bytes[(LabeledLength -12) : (LabeledLength)]
                label1 = bytes[(LabeledLength -3)]
                label2 = bytes[(LabeledLength -2)]
                label3 = bytes[(LabeledLength -1)]
                print(label1 + " " + label2 + " " + label3)

