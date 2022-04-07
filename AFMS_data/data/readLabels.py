import csv

labels = []

with open('labels.csv', 'rb') as f:
    reader = csv.reader(f)
    for row in reader:
        labels.append(row)


for rows in labels:
    print(row[3])


print('done')


