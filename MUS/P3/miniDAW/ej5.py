#Jorge Bravo
#Juan Diego Martínez

import numpy as np
from consts import *
from tkinter import *
from slider import *
from adsr import *
from synthFM import *

class KarplusStrong:
    def __init__(self, frequency, fs, duration):
        self.buffer_length = int(fs / frequency)
        self.buffer = np.random.uniform(-1, 1, self.buffer_length)
        self.index = 0
        self.duration = duration

    def next(self):
        value = self.buffer[self.index]
        self.index += 1
        if self.index >= self.buffer_length:
            self.buffer[:-1] = self.buffer[1:]
            self.buffer[-1] = (self.buffer[0] + self.buffer[-2]) * 0.5
            self.index = 0
        return value

class Instrument:
    def __init__(self, tk, name="Karplus-Strong Piano", amp=0.2, frequency=440, fs=44100, duration=1.0):
        self.name = name
        self.fs = fs
        self.duration = duration

        frame = LabelFrame(tk, text=name, bg="#808090")
        frame.pack(side=LEFT)

        self.ampS = Slider(frame, 'amp', packSide=TOP,
                           ini=amp, from_=0.0, to=1.0, step=0.05)

        self.freqS = Slider(frame, 'frequency', packSide=TOP,
                            ini=frequency, from_=20.0, to=2000.0, step=10.0)

        # ADSR params con sus sliders
        frameADSR = LabelFrame(frame, text="ADSR", bg="#808090")
        frameADSR.pack(side=LEFT, fill="both", expand="yes", )
        self.attackS = Slider(frameADSR, 'attack',
                              ini=0.01, from_=0.0, to=0.5, step=0.005, orient=HORIZONTAL, packSide=TOP)

        self.decayS = Slider(frameADSR, 'decay',
                             ini=0.01, from_=0.0, to=0.5, step=0.005, orient=HORIZONTAL, packSide=TOP)

        self.sustainS = Slider(frameADSR, 'sustain',
                               ini=0.2, from_=0.0, to=1.0, step=0.01, orient=HORIZONTAL, packSide=TOP)

        self.releaseS = Slider(frameADSR, 'release',
                               ini=0.5, from_=0.0, to=4.0, step=0.05, orient=HORIZONTAL, packSide=TOP)

        # canales indexados por la nota de lanzamiento -> solo una nota del mismo valor
        self.channels = dict()

    # obtenemos todos los parámetros del sinte (puede servir para crear presets)
    def getConfig(self):
        return (self.ampS.get(), self.freqS.get(),
                self.attackS.get(), self.decayS.get(), self.sustainS.get(),
                self.releaseS.get())

    # activación de nota
    def noteOn(self, midiNote):
        # si está el dict de canales reactivamos synt -> reiniciamos adsr del synt
        if midiNote in self.channels:
            self.channels[midiNote].start()

        # si no está, miramos frecuencia en la tabla de frecuencias
        # y generamos un nuevo synth en un canal indexado con notaMidi
        # con los parámetros actuales del synth
        else:
            freq = self.freqS.get()
            self.channels[midiNote] = KarplusStrong(
                frequency=freq, fs=self.fs, duration=self.duration)

    # apagar nota -> propagamos noteOff al synth, que se encargará de hacer el release
    def noteOff(self, midiNote):
        if midiNote in self.channels:  # está el dict, release
            del self.channels[midiNote]

    # lectura de teclas de teclado como eventos tkinter
    def down(self, event):
        c = event.keysym

        # tecla "panic" -> apagamos todos los sintes de golpe!
        if c == '0':
            self.stop()
        elif c in teclas:
            midiNote = 48 + teclas.index(c)  # buscamos indice y trasnportamos a C3 (48 en midi)
            self.noteOn(midiNote)  # arrancamos noteOn con el instrumento
            print(f'noteOn {midiNote}')

    def up(self, event):
        c = event.keysym
        if c in teclas:
            midiNote = 48 + teclas.index(c)  # buscamos indice y hacemos el noteOff
            self.noteOff(midiNote)

    # siguiene chunck del generador: sumamos señal de canales y hacemos limpia de silenciados
    def next(self):
        out = np.zeros(CHUNK)
        for c in list(self.channels):  # convertimos las keys a lista para mantener la lista de claves original
            out += self.channels[c].next()
        return out

    # boton del pánico
    def stop(self):
        self.channels = dict()  # delegamos en el garbage collector
