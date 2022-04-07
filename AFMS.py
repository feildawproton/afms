from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import gzip
import os
import re
import sys
import tarfile

from six.moves import urllib
import tensorflow as tf

import AFMS_input

FLAGS = tf.app.flags.FLAGS

# Basic model parameters.  I think I'd like to change the batch size
tf.app.flags.DEFINE_integer('batch_size', 32,
                            """Number of images to process in a batch.""")
# '/tmp/cifar10_data'
tf.app.flags.DEFINE_string('data_dir', './AFMS_data/',
                           """Path to the AFMS data directory.""")

#yea I can't do this efficiently on my graphics card, so leave as false
tf.app.flags.DEFINE_boolean('use_fp16', False,
                            """Don't train the model using fp16.""")

NUM_CLASSES = AFMS_input.AFMS_LABEL_CLASSES
NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN = AFMS_input.NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN
NUM_EXAMPLES_PER_EPOCH_FOR_EVAL = AFMS_input.NUM_EXAMPLES_PER_EPOCH_FOR_EVAL

# Constants describing the training process. I TOOK THESE DIRECTLY FROM THE EXAMPLE
MOVING_AVERAGE_DECAY = 0.9999     # The decay to use for the moving average.
NUM_EPOCHS_PER_DECAY = 350.0      # Epochs after which learning rate decays.
LEARNING_RATE_DECAY_FACTOR = 0.1  # Learning rate decay factor.
INITIAL_LEARNING_RATE = 0.1      # Initial learning rate.

first_fkernal = [2, 6, 6, 1, 32]  #basically three times the example but i don't know how channels affects whatever
first_pkernal = [1, 3, 3, 3, 1]  #uh not sue
first_pstride = [1, 2, 2, 2, 1]
#have not put a lot of thought into either of these
second_fkernal = [2, 6, 6, 32, 32]  
second_pkernal = [1, 3, 3, 3, 1]  
second_pstride = [1, 2, 2, 2, 1]

TOWER_NAME = 'tower'

def distorted_inputs():
    if not FLAGS.data_dir:
        raise ValueError("missing data dir or whatever")
    data_dir = FLAGS.data_dir #this should be fine
    videos, labels = AFMS_input.distorted_inputs(data_dir, batch_size = FLAGS.batch_size)

    return videos, labels;


#THIS FUNCTION IS JUST A MILD INDIRECTION! CONSIDER REMOVING IT
def inputs(eval_data):

    if not FLAGS.data_dir:
        raise ValueError("missing data dir or whatever")
    data_dir = FLAGS.data_dir #this should be fine
    videos, labels = AFMS_input.inputs(eval_data, data_dir, batch_size=FLAGS.batch_size)
    
    #i'm not even going to check for this because I don't have a lot of fp16 capability on my gaming graphics card    
    return videos, labels;
    

def _variable_with_weight_decay(name, shape, stddev, wd):
  """Helper to create an initialized Variable with weight decay.

  Note that the Variable is initialized with a truncated normal distribution.
  A weight decay is added only if one is specified.

  Args:
    name: name of the variable
    shape: list of ints
    stddev: standard deviation of a truncated Gaussian
    wd: add L2Loss weight decay multiplied by this float. If None, weight
        decay is not added for this Variable.

  Returns:
    Variable Tensor
  """
  dtype = tf.float16 if FLAGS.use_fp16 else tf.float32
  var = _variable_on_cpu(
      name,
      shape,
      tf.truncated_normal_initializer(stddev=stddev, dtype=dtype))
  if wd is not None:
    weight_decay = tf.mul(tf.nn.l2_loss(var), wd, name='weight_loss')
    tf.add_to_collection('losses', weight_decay)
  return var

def _variable_on_cpu(name, shape, initializer):
  """Helper to create a Variable stored on CPU memory.

  Args:
    name: name of the variable
    shape: list of ints
    initializer: initializer for Variable

  Returns:
    Variable Tensor
  """
  with tf.device('/cpu:0'):
    dtype = tf.float16 if FLAGS.use_fp16 else tf.float32
    var = tf.get_variable(name, shape, initializer=initializer, dtype=dtype)
  return var

def _activation_summary(x):
  """Helper to create summaries for activations.

  Creates a summary that provides a histogram of activations.
  Creates a summary that measures the sparsity of activations.

  Args:
    x: Tensor
  Returns:
    nothing
  """
  # Remove 'tower_[0-9]/' from the name in case this is a multi-GPU training
  # session. This helps the clarity of presentation on tensorboard.
  tensor_name = re.sub('%s_[0-9]*/' % TOWER_NAME, '', x.op.name)
  tf.summary.histogram(tensor_name + '/activations', x)
  tf.summary.scalar(tensor_name + '/sparsity', tf.nn.zero_fraction(x))


def inference(videos):
    #FIRST CONVOLUTION
    with tf.variable_scope('conv1') as scope:
        kernal = _variable_with_weight_decay('weights', first_fkernal, stddev = 5e-2, wd=0.0) #just using same stuff here
        #performing a 3d convolution instead of a 2d one
        #videos = tf.reshape(videos, [0, 2, 3, 4, 1])  
        print(videos)      
        conv = tf.nn.conv3d(videos, kernal, [1, 1, 1, 1, 1], padding='SAME')
        biases = _variable_on_cpu('biases', [32], tf.constant_initializer(0.0)) #first and last initializers at 0.0 the middle ones are at 0.1
        pre_activation = tf.nn.bias_add(conv, biases)
        conv1 = tf.nn.relu(pre_activation, name = scope.name)
        _activation_summary(conv1)
    
    #FIRST POOLING AND NORMALIZATION
    pool1 = tf.nn.max_pool3d(conv1, first_pkernal, first_pstride, padding='SAME', name='pool1')
    #AH I REALLY WANT A 3D (5D TENSOR) VERSION OF THIS
    #norm1 = tf.nn.lrn(pool1, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75, name = 'norm1')
    norm1 = tf.nn.l2_normalize(pool1, 1, epsilon = 1e-12, name = 'norm1') #i think dim = 0 means the batch dimension

    #SECOND CONVOLUTION
    with tf.variable_scope('conv2') as scope:
        kernal = _variable_with_weight_decay('weights', second_fkernal, stddev = 5e-2, wd=0.0) #just using same stuff here
        #AH I REALLY WANT A 3D (5D TENSOR) VERSION OF LOCAL RESPONSE NORMILIZATION
        #conv = tf.nn.conv3d(norm1, kernal, [1,1,1,1,1], padding='SAME')
        conv = tf.nn.conv3d(norm1, kernal, [1,1,1,1,1], padding='SAME')
        biases = _variable_on_cpu('biases', [32], tf.constant_initializer(0.1))
        pre_activation = tf.nn.bias_add(conv, biases)
        conv2 = tf.nn.relu(pre_activation, name=scope.name)

    #SECOND NORMALIZATION AND POOING
    #NO 5D TENSOR VERSION OF THIS
    #norm2 = tf.nn.lrn(conv2, 4, bias = 1.0, alpha = 0.001 / 9.0, beta = 0.75, name = 'norm2')
    norm2 = tf.nn.l2_normalize(conv2, 1, epsilon=1e-12, name='norm2')
    pool2 = tf.nn.max_pool3d(norm2, second_pkernal, second_pstride, padding='SAME', name = 'pool2')

    #FIRST LOCAL, LAYER 3
    with tf.variable_scope('local3') as scope:
        #reshape to make a single matrix multiply
        reshape = tf.reshape(pool2, [FLAGS.batch_size, -1])
        dim = reshape.get_shape()[1].value
        #just using vals from example here
        weights = _variable_with_weight_decay('weights', shape = [dim, 384], stddev = 0.04, wd = 0.004) 
        biases = _variable_on_cpu('biases', [384], tf.constant_initializer(0.1))
        local3 = tf.nn.relu(tf.matmul(reshape, weights) + biases, name = scope.name)
        _activation_summary(local3)
        
    #SECOND LOCAL, LAYER 4
    with tf.variable_scope('local4') as scope:
        weights = _variable_with_weight_decay('weights', shape = [384, 192], stddev = 0.04, wd = 0.004) #vars from example
        biases = _variable_on_cpu('biases', [192], tf.constant_initializer(0.1))
        local4 = tf.nn.relu(tf.matmul(local3, weights) + biases, name=scope.name)
        _activation_summary(local4)

    #LINEAR LAYER, SPARSE_SOFTMAX_CROSS_ENTROPY_WITH_LOGITS PERFORMS SOFTMAX INTERNALLY
    with tf.variable_scope('softmax_linear') as scope:
        weights = _variable_with_weight_decay('weights', [192, NUM_CLASSES], stddev = 1/192.0, wd = 0.0)
        biases = _variable_on_cpu('biases', [NUM_CLASSES], tf.constant_initializer(0.0))
        softmax_linear = tf.add(tf.matmul(local4, weights), biases, name = scope.name)
        _activation_summary(softmax_linear)

    return softmax_linear;

def loss(logits, labels):
    #calc average cross entropy loss across the batch
    labels = tf.cast(labels, tf.int64)
    cross_entropy = tf.nn.sparse_softmax_cross_entropy_with_logits(logits, labels, name = 'cross_entropy_per_example')
    cross_entropy_mean = tf.reduce_mean(cross_entropy, name='cross_entropy')
    tf.add_to_collection('losses', cross_entropy_mean)
    
    return tf.add_n(tf.get_collection('losses'), name = 'total_loss')


def _add_loss_summaries(total_loss):
  """Add summaries for losses in CIFAR-10 model.

  Generates moving average for all losses and associated summaries for
  visualizing the performance of the network.

  Args:
    total_loss: Total loss from loss().
  Returns:
    loss_averages_op: op for generating moving averages of losses.
  """
  # Compute the moving average of all individual losses and the total loss.
  loss_averages = tf.train.ExponentialMovingAverage(0.9, name='avg')
  losses = tf.get_collection('losses')
  loss_averages_op = loss_averages.apply(losses + [total_loss])

  # Attach a scalar summary to all individual losses and the total loss; do the
  # same for the averaged version of the losses.
  for l in losses + [total_loss]:
    # Name each loss as '(raw)' and name the moving average version of the loss
    # as the original loss name.
    tf.summary.scalar(l.op.name +' (raw)', l)
    tf.summary.scalar(l.op.name, loss_averages.average(l))

  return loss_averages_op


#train the model
def train(total_loss, global_step):
    #variables
    num_batches_per_epoch = NUM_EXAMPLES_PER_EPOCH_FOR_TRAIN / FLAGS.batch_size
    decay_steps = int(num_batches_per_epoch * NUM_EPOCHS_PER_DECAY)

    #decay learning rate
    lr = tf.train.exponential_decay(INITIAL_LEARNING_RATE, global_step, decay_steps, LEARNING_RATE_DECAY_FACTOR, staircase = True)
    
    tf.summary.scalar('learning_rate', lr)

    loss_averages_op = _add_loss_summaries(total_loss)

    #computate grdients
    with tf.control_dependencies([loss_averages_op]):
        opt = tf.train.GradientDescentOptimizer(lr)
        grads = opt.compute_gradients(total_loss)

    #apply
    apply_gradient_op = opt.apply_gradients(grads, global_step = global_step)

    #histogram
    for var in tf.trainable_variables():
        tf.summary.histogram(var.op.name, var)

    for grad, var in grads:
        if grad is not None:
            tf.summary.histogram(var.op.name + '/gradients', grad)

    #track averages with trainables variables
    variable_averages = tf.train.ExponentialMovingAverage(MOVING_AVERAGE_DECAY, global_step)
    variables_averages_op = variable_averages.apply(tf.trainable_variables())

    with tf.control_dependencies([apply_gradient_op, variables_averages_op]):
        train_op = tf.no_op(name = 'train')

    return train_op;

#vids, labs = distorted_inputs()
#logits = inference(vids)
#lss = loss(logits, labs)

#global_step = tf.Variable(0, trainable=False)

#trn_op = train(lss, global_step)

#inputs(True)
#inputs(False)

#print('done')






