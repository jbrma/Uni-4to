#   Jorge Bravo y Juan Diego Martínez

import numpy as np
import sounddevice as sd
import kbhit

SRATE = 44100
CHUNK = 1024

class Osc:
    def __init__(self, frequency=440, volume=0.5, phase=0):
        self.frequency = frequency
        self.volume = volume
        self.phase = phase
        self.current_frame = 0

    def next(self, chunk_size=1024, sample_rate=44100):
        data = self.volume*np.sin(2*np.pi*(np.arange(self.current_frame,self.current_frame+CHUNK))*self.frequency/SRATE)
        self.current_frame += chunk_size
        return np.float32(data)

    def set_frequency(self, frequency):
        self.frequency = frequency

    def get_frequency(self):


        return self.frequency

    def set_volume(self, volume):
        self.volume = volume

    def get_volume(self):
        return self.volume

def testOsc():
    osc = Osc(frequency=440, volume=0.5)

    stream = sd.OutputStream(samplerate=SRATE, blocksize=CHUNK, channels=1)
    stream.start()
    kb = kbhit.KBHit()
    
    fin = False
    freq = 100
    vol = 0.1

    while not(fin):
        samples = osc.next()
        stream.write(samples)

        c = kb.getKey()
        if c!=' ':
            if c in ['q','escape']: break
            elif c=='v': 
                osc.set_volume(min(max(0, osc.get_volume()-vol), 10))
            elif c=='V': 
                osc.set_volume(min(max(0, osc.get_volume()+vol), 10))
            elif c=='f': 
                osc.set_frequency(min(max(20, osc.get_frequency()-freq), 20000))
            elif c=='F': 
                osc.set_frequency(min(max(20, osc.get_frequency()+freq), 20000))
    
    kb.quit()
    stream.stop()
    stream.close()


testOsc()
