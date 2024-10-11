
from tkinter import *
from consts import *
import sounddevice as sd
import numpy as np
from instrument import *

# parser de notacion ABC. Descargado de https://github.com/campagnola/pyabc
from pyabc import *

class ABCsequencer:    
    # unit es un escalado temporal, permite subir/bajar el tempo
    def __init__(self,tk,unit=1000,instrument=None):                          
        if instrument == None:            
            self.instrument = Instrument(tk)
        else:
            self.instrument = instrument

        self.unit = unit # unidad de escalado para el tempo

        # ventanas para el secuenciador, carga de canción y botones play/stop
        frame = LabelFrame(tk, text="ABC Sequencer", bg="#908060")
        frame.pack(side=TOP)

        frameFile = Frame(frame, highlightbackground="blue", highlightthickness=6)
        frameFile.pack(side=TOP)
        Label(frameFile,text='Archivo ABC: ').pack(side=LEFT)
 
        self.file = Entry(frameFile) 
        self.file.insert(14,"cumple.abc")
        self.file.pack(side=LEFT)
        
        song = self.getSong(self.file.get())
        self.seq = self.songToSeq(song)           


        self.text = Text(frame,height=6,width=23)
        self.text.pack(side=RIGHT)
        playBut = Button(frame,text="Play", command=self.play)
        playBut.pack(side=TOP)
        stopBut = Button(frame,text="Stop", command=self.stop)
        stopBut.pack(side=BOTTOM)

        self.tick = 1  # retardo de tiempo para el bucle de secuenciación
        self.state = 'off' # estado del secuenciador

    # pasa de notación ABC a lista (nota,duración)
    def getSong(self,file):
         # abrimos archivo y parseamos
        text = open(file,'r').read()
        song = Tune(text)

        # nombre de las notas de 2 octavas, con '.' para sostenidos
        notas = "C.D.EF.G.A.Bc.d.ef.g.a.bc."
        
        # transformamos a lista de pares (nota,duración)
        partitura = []
        for n in song.tokens: 
            if isinstance(n,Note):
                nota = notas.index(n.note)
                nota, o = nota%12, nota//12
                partitura.append((notas[nota+12*o],n.duration))
        print(f"Lista de notas: {partitura}")
        return partitura

        
    # genera la secuencia de eventos (time,event,value)
    # esto sirve para secuencias de notas: melodias (no hay polifónia)
    def songToSeq(self,song):
        seq = []
        time = 0 # tiempo acumulado
        # para cada nota añadimos noteOn/noteOff con sus tiempos
        for (note,dur) in song:            
            seq.append((time,'noteOn',note))
            time += dur*self.unit # incremento temporal * escalado
            seq.append((time,'noteOff',note))
        return seq
    

    # incio del secuenciador    
    def play(self):
        self.state = 'on'
        self.playLoop()

    # método recursivo que va avanzando en el tiempo
    def playLoop(self,item=0,time=0):   
        
        # termina la secuancia o paran el secuenciador
        if item>=len(self.seq) or self.state =='off':
            return
        
        # si le toda al siguiente ítem
        if time>=self.seq[item][0]:
            (_,msg,note) = self.seq[item]
            index = notas.index(note) 
            if msg=='noteOn': # lanzamos nota al instrumento           
                self.instrument.noteOn(index+48)
                self.text.insert('6.0',  f'{msg} {note}')            
            else: # msg noteOf, la paramos   
                self.instrument.noteOff(index+48)
                self.text.insert('6.0',  f' - {msg} {note}\n')            
            item += 1 # se avanza al siguiente ítem 

        # el tiempo avanza en cualquier caso    
        time += self.tick

        # programamos llamada recursiva
        self.text.after(self.tick,lambda: self.playLoop(item,time)) 
         

    def stop(self):
        self.instrument.stop()
        self.state = 'off'          
