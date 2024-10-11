#   Jorge Bravo y Juan Diego Martínez

# %%

import numpy as np
import sounddevice as sd
import soundfile as sf
import matplotlib.pyplot as plt
import kbhit


SRATE = 44100
CHUNK = 1024

freqs={'C':523.251, 'D':587.33, 'E':659.255, 'F':698.456, 'G':783.991, 'A':880, 'B':987.767}

partitura = [(freqs.get('G'), 0.5), (freqs.get('G'), 0.5), (freqs.get('A'), 1), (freqs.get('G'), 1), (freqs.get('C')*2, 1), (freqs.get('B'), 2),
             (freqs.get('G'), 0.5), (freqs.get('G'), 0.5), (freqs.get('A'), 1), (freqs.get('G'), 1), (freqs.get('D')*2, 1), (freqs.get('C')*2, 2),
             (freqs.get('G'), 0.5), (freqs.get('G'), 0.5), (freqs.get('G')*2, 1), (freqs.get('E')*2, 1), 
             (freqs.get('C')*2, 1), (freqs.get('B'), 1), (freqs.get('A'), 1),
             (freqs.get('F')*2, 0.5), (freqs.get('F')*2, 0.5), (freqs.get('E')*2, 1), (freqs.get('C')*2, 1),
             (freqs.get('D')*2, 1), (freqs.get('C')*2, 2) ]

class Osc:
    def __init__(self):
        self.isPlaying = False

    def noteOn(self):
        self.isPlaying = False

    def noteOff(self):
        self.isPlaying = True

def osc(freq,dur,vol):
 return np.float32(vol*np.sin(2*np.pi*np.arange(SRATE*dur)*freq/SRATE))

def scheduler(partitura):
    oscilador = Osc()
    output = np.array([])

    for nota in partitura:
        freq, dur = nota

        if not oscilador.isPlaying:
            oscilador.noteOn()
            output = np.append(output, osc(freq, dur/2, 0.5))
        else:
            oscilador.noteOff()
           
    
    sd.play(output)

scheduler(partitura)

# %%
#%% Ejercicio 4
last = 0

def oscDur(freq,dur,vol):
    
    global last 

    data = vol*np.sin(2*np.pi*(np.arange(last,last+dur))*freq/SRATE)
    last += dur # actualizamos ultimo generado
    return np.float32(data)


#Idea. dibujar todas las ondas que hay en el rango de frecIni a frecFin dividiendo el tiempo total entre el numero de freq que hay para representarlas todas 
def chirp(frecIni,frecFin,dur):
    dif = frecFin-frecIni
    tiempOsc= dur/dif #Calculamos cuanto tiene que durar cada onda
    total = oscDur(frecIni,tiempOsc,0.5)
    for i in range(frecIni+1, frecFin+1):
        total = np.append(total, oscDur(i,tiempOsc,0.5)) 
       #concatenamos cada onda con la anterior para obtener la onda total
    return total

vaina  = chirp(100,400,4000)
plt.plot(vaina)

# %%
plt.plot(osc(100, 2, 0.5))
plt.plot(osc(100, 1, 0.5))

# %%
