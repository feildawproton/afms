from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from PIL import Image
import os
import tensorflow as tf

from random import randint #for testing and such
import ntpath #should work on all platforms apparently

#NEED TO CHANGE THIS STUFF BETWEEN TESTINGS
NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN = 799 #there are 799 examples from B
NUM_EXAMPLES_PER_EPOCH_FOR_EVAL = 296 # there are 296 examples from P

AFMS_LABEL_TOUSE = 3 #options are 1, 2, and 3
AFMS_LABEL_CLASSES = 5  #8 for AFMS_LABEL_TOUSE = 1, 13 for AFMS_LABEL_TOUSE = 2, and 5 for AFMS_LABEL_TOUSE = 3

LABELED_DATA_BYTES = 16396
DATA_BYTES = 16384
NUM_LABELS = 3
BYTES_PER_LABEL = 4
LABEL_BYTES = NUM_LABELS * NUM_LABELS

VIDEO_DEPTH = 16 #meh
VIDEO_SIZE = 32 #don't know why I decided to keep this as a separate variable
VIDEO_CHANNELS = 1 # alright apparently I didn't need to reduc this but imma keep it this way

def read_singleAFMS(fileName):  #treating fileTuple like filename_queue in example
    #reads a single AFMS example
    #what it does is convert the collection of tiffs into a sinle image with depth 27
    #along the way we down convert the images from 24bit to 8bit because they are 

    #the fileTuple is the folder containing the images
    #this is from os.walk [0] is the name and [2] is the array of image names

    #we need to sort the 

    class singleAFMSRecord(object):
        pass
    result = singleAFMSRecord()

    #giveLabel = randint(1,7) #jeeze i'll figure this out later
    #print(fileName)
    #result.label = tf.cast(giveLabel, tf.int32)
    
    result.depth = VIDEO_DEPTH
    result.height = VIDEO_SIZE
    result.width = VIDEO_SIZE
    result.channels = VIDEO_CHANNELS

    result.labels = NUM_LABELS
    result.labelSize = BYTES_PER_LABEL

    if(LABELED_DATA_BYTES != ( (result.height*result.width*result.depth*result.channels) + (result.labels*result.labelSize) )):
        print("messed up the size calculation")

    record_reader = tf.FixedLengthRecordReader(LABELED_DATA_BYTES)
    result.key, value = record_reader.read(fileName)

    print(result.key)

    #create the tensor or whatever
    record_bytes = tf.decode_raw(value, tf.uint8)

    label_slice1 = tf.slice(record_bytes, [DATA_BYTES], [1])
    label_slice2 = tf.slice(record_bytes, [DATA_BYTES + BYTES_PER_LABEL], [1])
    label_slice3 = tf.slice(record_bytes, [DATA_BYTES + (2 * BYTES_PER_LABEL)], [1])

    #because the freekin thing above is a 2d tensor or whatever

    label1 = label_slice1[0]
    label2 = label_slice2[0]
    label3 = label_slice3[0]

    if(AFMS_LABEL_TOUSE == 1):

        if(AFMS_LABEL_CLASSES != 8):
            print('label type and class mismatch')
        result.label = tf.cast(label1, tf.int32)

    elif(AFMS_LABEL_TOUSE == 2):

        if(AFMS_LABEL_CLASSES != 13):
            print('label type and class mismatch')
        result.label = tf.cast(label2, tf.int32)

    elif(AFMS_LABEL_TOUSE == 3):

        if(AFMS_LABEL_CLASSES != 5):
            print('label type and class mismatch')
        result.label = tf.cast(label3, tf.int32)

    else:
        result.label = tf.cast(0, tf.int32)
        print('Error in label selection logic')
    
    #debbuging this shit

     

    print(result.label)

    giverandom = randint(1,255)
    result.label = tf.cast(giverandom, tf.int32)

    print(result.label)
 
    data_slice = tf.slice(record_bytes, [0], [DATA_BYTES])
    
    depthMajor_tensor = tf.reshape(data_slice,  [VIDEO_DEPTH, VIDEO_SIZE, VIDEO_SIZE, VIDEO_CHANNELS])

    result.uint8video = depthMajor_tensor
    
    return result;



def _generate_video_and_label_batch(video, label, min_queue_examples, batch_size, shuffle):
    #image cube should be 81 81 27 by now
    #shuffle examples then read from queue

    print(label)
    num_preprocess_threads = 16 #more than I got... (8)
    if(shuffle):
        videos, label_batch = tf.train.shuffle_batch(
        [video, label], 
        batch_size = batch_size, 
        num_threads=num_preprocess_threads, 
        capacity = min_queue_examples + 3 * batch_size,  #had to remove the 3*batch_size here
        min_after_dequeue = min_queue_examples)

        print(batch_size)
        print(min_queue_examples + 3 * batch_size)

    else:
        videos, label_batch = tf.train.batch(
        [video, label], 
        batch_size = batch_size, 
        num_threads=num_preprocess_threads, 
        capacity = min_queue_examples + 3 * batch_size)

    #some kind of display
    #tf.summary.image('images', videos)
    
    return videos, tf.reshape(label_batch, [batch_size]);

#because tf doesn't have the dame distortion built in for video i;m skipping these transformations
#so distorted_input is not different from inputs()
def distorted_inputs(data_dir, batch_size):
    #lets do this
    trainDir = data_dir + "Train/"  #we know we will be using the training directory if we are distoring stuff
    files = os.walk(trainDir)

    filenames = []
    count = 0
    for fileTuple in files:
        if(len(fileTuple[2]) !=0):
            for name in fileTuple[2]:
                fullName = os.path.join(fileTuple[0], name)
                filenames.append(fullName)          
                count += 1


    print(str(len(filenames)) + " training videos")

    #i guess we need a queu
    filename_queu = tf.train.string_input_producer(filenames) 

    read_input = read_singleAFMS(filename_queu)
    reshaped_video = tf.cast(read_input.uint8video, tf.float32)  #because floats are what this GC does well i think
  
    #lets do some distortion apparently
    #doesn't work for 4d tensors
    #distorted_video = tf.random_crop(reshaped_video, [RECORD_DEPTH, RECORD_HEIGHT, RECORD_WIDTH])
    #distorted_video = tf.image.random_flip_left_right(distorted_video)

    #adding randomness to the next to operations because they are not commutative apparently
    #randy = randint(0,1)
    #if(randy == 0):
    #    distorted_image = tf.image.random_brightness(distorted_image, max_delta=63) #where does 63 come frome?!
    #    distroted_image = tf.image.random_contrast(distorted_image, lower = 0.2, upper = 1.8) #numbers?!
    #else:
    #    distroted_image = tf.image.random_contrast(distorted_image, lower = 0.2, upper = 1.8) #numbers?!
    #    distorted_image = tf.image.random_brightness(distorted_image, max_delta=63) #where does 63 come frome?!

    #float_image = tf.image.per_image_standardization(distorted_image) #normalization per image?
    

    # Ensure that the random shuffling has good mixing properties.
    min_fraction_of_examples_in_queue = 0.4
    min_queue_examples = int(NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN * min_fraction_of_examples_in_queue)
    print ('Filling queue with %d AFMS videos before starting to train.' % min_queue_examples)

    

    return _generate_video_and_label_batch(
    reshaped_video,
    read_input.label,
    min_queue_examples,
    batch_size,
    shuffle=True)


def inputs(eval_data, data_dir, batch_size):
    #use this for testing data and such
    
    if not eval_data:
        data_dir_dir = data_dir + "Train"
        num_examples_per_epoch = NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN
    else:
        data_dir_dir = data_dir + "Test"
        num_examples_per_epoch = NUM_EXAMPLES_PER_EPOCH_FOR_EVAL

    files = os.walk(data_dir_dir)

    filenames = []
    count = 0
    for fileTuple in files:
        if(len(fileTuple[2]) !=0):
            for name in fileTuple[2]:
                fullName = os.path.join(fileTuple[0], name)
                filenames.append(fullName)
                count += 1


    print(str(len(filenames)) + " training videos")

    #i guess we need a queu
    filename_queu = tf.train.string_input_producer(filenames) 
    read_input = read_singleAFMS(filename_queu)
    reshaped_video = tf.cast(read_input.uint8video, tf.float32)  #because floats are what this GC does well i think
  
    #lets do some distortion apparently
    resized_video = tf.random_crop(reshaped_video, [RECORD_DEPTH, RECORD_HEIGHT, RECORD_WIDTH, RECORD_CHANNELS])

    #float_image = tf.image.per_image_standardization(resized_image) #normalization per image?
    #float_video = resized_video
     # Ensure that the random shuffling has good mixing properties.
    min_fraction_of_examples_in_queue = 0.4
    min_queue_examples = int(num_examples_per_epoch * min_fraction_of_examples_in_queue)
    print ('Filling queue with %d AFMS videos before starting to train.' % min_queue_examples)
    
    return _generate_video_and_label_batch(
    reshaped_video,
    read_input.label,
    min_queue_examples,
    batch_size,
    shuffle=False)




#distorted_inputs('./AFMS_data/', batch_size = 128)
#print('got here')
#inputs(True, './AFMS_data/', batch_size = 128)
#print('got here')
#inputs(False, './AFMS_data/', batch_size = 128)
#print('got here')


    #print(count)
    #just to check that this works like I hope it did
    #eighteenStart = (RECORD_WIDTH*RECORD_HEIGHT*18)
    #eighteenEnd = (RECORD_WIDTH*RECORD_HEIGHT*19)
    #rawEighteen = depthMajor_data_str[eighteenStart:eighteenEnd]
    
    #imEighteen = Image.frombytes("L", [RECORD_WIDTH, RECORD_HEIGHT], rawEighteen)
    #imEighteen.show()
