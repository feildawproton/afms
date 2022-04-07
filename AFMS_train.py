#we need this for reasons
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import tensorflow as tf

import AFMS

from six.moves import xrange
from datetime import datetime
import time
import os.path

import numpy as np

FLAGS = tf.app.flags.FLAGS # a shortcut
tf.app.flags.DEFINE_string('train_dir', './AFMS_train',
                           """Directory where to write event logs """
                           """and checkpoint.""")
# max_steps was originally 1 million, i have changed for testing
# the paper says 100,000 though...
tf.app.flags.DEFINE_integer('max_steps', 60000,
                            """Number of batches to run.""")
tf.app.flags.DEFINE_boolean('log_device_placement', False,
                            """Whether to log device placement.""")

def train():
    #with some defaults apparenly
    with tf.Graph().as_default():
        global_step = tf.Variable(0, trainable = False) #uh

        videos, labels = AFMS.distorted_inputs() #alright lets get the stuff

        logits = AFMS.inference(videos)
        loss = AFMS.loss(logits, labels)
        train_op = AFMS.train(loss, global_step)

        #a saver
        saver = tf.train.Saver(tf.global_variables())
        #summaries
        summary_op = tf.summary.merge_all()
        #need an initializer for later
        init = tf.global_variables_initializer()

        #start a session running the ops
        sess = tf.Session(config = tf.ConfigProto(log_device_placement = FLAGS.log_device_placement))
        sess.run(init)
        
        #start queue runners
        tf.train.start_queue_runners(sess=sess)

        summary_writer = tf.summary.FileWriter(FLAGS.train_dir, sess.graph)

        print('got here')

        for step in xrange(FLAGS.max_steps):
            start_time = time.time()
            _, loss_value = sess.run([train_op, loss])


            duration = time.time() - start_time
           
            #oops
            #assert not np.isnan(loss_value), 'Model diverged with loss == nan'
            
            if step % 10 == 0:              

                num_examples_per_step = FLAGS.batch_size
                examples_per_sec = num_examples_per_step / duration
                sec_per_batch = float(duration)
                format_str = ('%s: step %d, loss = %.2f (%.1f examples/sec; %.3f ' 'sec/batch)')
                print (format_str % (datetime.now(), step, loss_value, examples_per_sec, sec_per_batch))

#            if step % 100 == 0:
#                summary_str = sess.run(summary_op)
#                summary_writer.add_summary(summary_str, step)

            #save periodically
            if step % 1000 == 0 or (step + 1) == FLAGS.max_steps:
                checkpoint_path = os.path.join(FLAGS.train_dir, 'model.ckpt')
                saver.save(sess, checkpoint_path, global_step = step)

#if it exists get rid of it
if(tf.gfile.Exists(FLAGS.train_dir)):
    tf.gfile.DeleteRecursively(FLAGS.train_dir)
#make it
tf.gfile.MakeDirs(FLAGS.train_dir)
#call train

train()
