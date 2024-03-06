import time

class ChannelControl:
  def __init__(self, volume):
    self.volume = volume
    self.t0 = 0.0 # start time of fade in/out
    self.fade_function = lambda t : t
    self.is_fading_in = False
    self.is_fading_out = False

  def start_fade_in(self, duration: float):
    start_vol = self.volume
    self.t0 = time.time()
    self.is_fading_out = False
    self.is_fading_in = True
    self.fade_function = lambda t : min(1.0, start_vol + 1.0/duration * t)
  
  def start_fade_out(self, duration: float):
    start_vol = self.volume
    self.t0 = time.time()
    self.is_fading_in = False
    self.is_fading_out = True
    self.fade_function = lambda t : max(0.0, start_vol - 1.0/duration * t)
  
  def stop_fade(self):
    self.is_fading_in = False
    self.is_fading_out = False

  def is_activated(self):
    return self.volume == 1

  def is_deactivated(self):
    return self.volume == 0

  def is_changing(self):
    return self.is_fading_in or self.is_fading_out

  def update(self):
    t1 = time.time()
    dt = t1 - self.t0

    self.volume = self.fade_function(dt)

    if self.volume == 0 or self.volume == 1:
      self.stop_fade()