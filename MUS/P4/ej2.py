# Jorge Bravo
# Juan Diego Martínez

from instrument import *


class PadInC(Instrument):
    def noteOn(self, midiNote):
        if midiNote in self.channels:
            self.channels[midiNote].start()
        else:
            root_freq = freqsMidi[midiNote]
            fifth_note = midiNote + 7 if midiNote != 11 else 12 
            fifth_freq = freqsMidi[fifth_note]

            self.channels[midiNote] = SynthFM(
                fc=root_freq,
                amp=self.ampS.get(), ratio=self.ratioS.get(), beta=self.betaS.get(),
                attack=self.attackS.get(), decay=self.decayS.get(),
                sustain=self.sustainS.get(), release=self.releaseS.get())

            self.channels[fifth_note] = SynthFM(
                fc=fifth_freq,
                amp=self.ampS.get(), ratio=self.ratioS.get(), beta=self.betaS.get(),
                attack=self.attackS.get(), decay=self.decayS.get(),
                sustain=self.sustainS.get(), release=self.releaseS.get())
    
    def noteOff(self, midiNote):
        if midiNote in self.channels:
            self.channels[midiNote].noteOff()

            fifth_note = midiNote + 7 if midiNote != 11 else 12
            if fifth_note in self.channels:
                self.channels[fifth_note].noteOff()


from tkinter import *
import sounddevice as sd


def test():
    def callback(outdata, frames, time, status):    
        if status: print(status)    
        s = np.sum([i.next() for i in inputs],axis=0)
        s = np.float32(s)
        outdata[:] = s.reshape(-1, 1)

    tk = Tk()
    ins = PadInC(tk)
    inputs = [ins]

    # desactivar repeticion de teclas
    #os.system('xset r off')

    stream = sd.OutputStream(samplerate=SRATE, channels=1, blocksize=CHUNK, callback=callback)    
    stream.start()
    tk.mainloop()

    # reactivar repeticion de teclas   
    #os.system('xset r on')
    stream.close()

test()    
