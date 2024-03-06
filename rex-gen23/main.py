from keras import backend as K
import imutils
from keras.models import load_model
import numpy as np
import keras
import requests
from scipy.spatial import distance as dist
from imutils import face_utils
import time
import dlib
import cv2,os,sys
import collections
import random
import face_recognition
import pickle
import math
import threading
from typing import List
import tensorflow as tf
from pygame import mixer

from ChannelControl import ChannelControl

mixin_toggle_threshold = 0.1

class FacialLandMarksPosition:
    """
    The indices points to the various facial features like left ear, right ear, nose, etc.,
    that are mapped from the Facial Landmarks useeditord by dlib's FacialLandmarks predictor.
    """
    left_eye_start_index, left_eye_end_index = face_utils.FACIAL_LANDMARKS_IDXS["left_eye"]
    right_eye_start_index, right_eye_end_index = face_utils.FACIAL_LANDMARKS_IDXS["right_eye"]

def predict_eye_state(model, image):
    image = cv2.resize(image, (20, 10))
    image = image.astype(dtype=np.float32)
        
    image_batch = np.reshape(image, (1, 10, 20, 1))
    image_batch = tf.keras.applications.mobilenet.preprocess_input(image_batch)

    return np.argmax( model.predict(image_batch)[0] )

def should_mixin_play(eye_state_queue: List[int]):
    sum = 0.0
    for s in eye_state_queue:
        sum += s
    avg = sum / len(eye_state_queue)
    # print(avg)

    return avg <= mixin_toggle_threshold

# Init model 
num_cores = 4

num_CPU = 1
num_GPU = 0


config = tf.compat.v1.ConfigProto(intra_op_parallelism_threads=num_cores,
                        inter_op_parallelism_threads=num_cores, 
                        allow_soft_placement=True,
                        device_count = {'CPU' : num_CPU,
                                        'GPU' : num_GPU}
                       )

session = tf.compat.v1.Session(config=config)
K.set_session(session)

facial_landmarks_predictor = './models/68_face_landmarks_predictor.dat'
predictor = dlib.shape_predictor(facial_landmarks_predictor)
 
model = load_model('./models/weights.149-0.01.hdf5')


# Init sounds
mixer.init()


soundscape_base = mixer.Sound('assets/soundscape_base.wav')
soundscape_1 = mixer.Sound('assets/soundscape_1.wav')
soundscape_2 = mixer.Sound('assets/soundscape_2.wav')
# soundscape_3 = mixer.Sound('assets/soundscape_3.wav')

soundscapes = [soundscape_1, soundscape_2]

channel_base = mixer.Channel(0)
channel_mixin = mixer.Channel(1)

control_base = ChannelControl(volume=1.0)
control_mixin = ChannelControl(volume=0.0)

channel_mixin.set_volume(control_mixin.volume)

channel_base.play(soundscape_base)

# Init video 
cap = cv2.VideoCapture(0)
scale = 0.5

eye_state_queue = []

t0 = time.time()

ss_index = 0

while(True):

    c = time.time()

    # Capture frame-by-frame
    ret, frame = cap.read()

    image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    original_height, original_width = image.shape[:2]

    resized_image = cv2.resize(image,  (0, 0), fx=scale, fy=scale)
    lab = cv2.cvtColor(resized_image, cv2.COLOR_BGR2LAB)

    l, _, _ = cv2.split(lab)

    resized_height, resized_width = l.shape[:2]
    height_ratio, width_ratio = original_height / resized_height, original_width / resized_width

    face_locations = face_recognition.face_locations(l, model='hog')

    if len(face_locations):
        top, right, bottom, left = face_locations[0]
        x1, y1, x2, y2 = left, top, right, bottom

        x1 = int(x1 * width_ratio)
        y1 = int(y1 * height_ratio)
        x2 = int(x2 * width_ratio)
        y2 = int(y2 * height_ratio)

        # draw face rectangle

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        shape = predictor(gray, dlib.rectangle(x1, y1, x2, y2))
        
        face_landmarks = face_utils.shape_to_np(shape)

        left_eye_indices = face_landmarks[FacialLandMarksPosition.left_eye_start_index:
                                          FacialLandMarksPosition.left_eye_end_index]

        right_eye_indices = face_landmarks[FacialLandMarksPosition.right_eye_start_index:
                                           FacialLandMarksPosition.right_eye_end_index]

        (x, y, w, h) = cv2.boundingRect(np.array([left_eye_indices]))
        left_eye = gray[y:y + h, x:x + w]

        (x, y, w, h) = cv2.boundingRect(np.array([right_eye_indices]))
        right_eye = gray[y:y + h, x:x + w]

        left_eye_open = predict_eye_state(model=model, image=left_eye)   
        right_eye_open = predict_eye_state(model=model, image=right_eye)   

        if len(eye_state_queue) == 10:
            eye_state_queue.pop(0)

        eye_state_queue.append(int(left_eye_open and right_eye_open))

        # [print(s, end=' | ') for s in eye_state_queue]
        # print('')
        
        if time.time() - t0 >= 10:
            if should_mixin_play(eye_state_queue):
                if not control_mixin.is_activated() and not control_mixin.is_fading_in:
                    channel_mixin.play(soundscapes[ss_index])
                    control_mixin.start_fade_in(duration=2)
                    control_base.start_fade_out(duration=25)
                    ss_index = (ss_index + 1) % 2
            else:
                if not control_mixin.is_deactivated() and not control_mixin.is_fading_out:
                    control_mixin.start_fade_out(duration=5)
                    control_base.start_fade_in(duration=5)

        if control_base.is_changing():
            control_base.update()
            vol_base = control_base.volume
            print('Base: ')
            print(vol_base)
            channel_base.set_volume(vol_base)
    
        if control_mixin.is_changing():
            control_mixin.update()
            vol_mixin = control_mixin.volume
            print('Mixin: ')
            print(vol_mixin)
            channel_mixin.set_volume(vol_mixin)

        
        if left_eye_open and right_eye_open:
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
        else:
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)

        cv2.imshow('right_eye', right_eye)
        cv2.imshow('left_eye', left_eye)

    cv2.imshow('frame', cv2.flip(frame, 1))


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
