import os

targetFile = open('names.txt', 'w')

paths = os.walk('.')

for pathTuple in paths:
    if(pathTuple[0] != '.'):
        for name in pathTuple[2]:
            targetFile.write(name)
            targetFile.write("\n")

targetFile.close()
