#%% celda inicial para cargar librerías y definir constantes

import matplotlib.pyplot as plt
import numpy as np
import time

# gráficos en el notebook
%matplotlib inline 

SRATE = 44100 # Sample rate, para todo el programa


#%%

def noise(dur):   
    # emtpy necesita argumento int 
    a = np.empty(round(dur*SRATE))
    for i in range(a.size):
        # convertimos valores de [0,1) en [-1,1)
        # a[i] = np.random.random()*2-1      

        # otra forma mád directa
        a[i] = np.random.uniform(-1,1)
    return a

start = time.time()
dur = 10
a = noise(dur)
print(f'time: {time.time() - start}')

# round(dur*SRATE): numero de muestras de la señal
print(f'Num muestras: {round(dur*SRATE)}')
plt.plot(a[:20])
#plt.plot(a[:20])



#%% una forma mucho más directa
# Delegamos todo el trabajo en NumPy
# Más eficiente?

def noise2(dur):
    # uniform necesita int
    return np.random.uniform(-1,1,round(dur*SRATE))

start = time.time()
dur = 10
a = noise2(dur)
print(f'time: {time.time() - start}')


plt.plot(a[:20])


#%%


# arange no necesita conversion a int de SRATE*dur 
dur = 1
freq = 1
a = np.sin(np.arange(SRATE*dur)*2*np.pi*freq/SRATE)

plt.plot(a)



#%%

def osc(freq=440, dur=1, amp=1, phase=0):
    a = np.arange(dur*SRATE)
    return amp*np.sin(phase+freq*a*2*np.pi/SRATE)

# b = osc(4,4,0.5,0)+0.5


# %%

def modulator(sample=noise2(1), freq=1):
    plt.plot(sample*(osc(1,1,0.5,0.75))+0.5)
    plt.plot(sample*(osc(freq,1,0.5,0))+0.5)
    

modulator(freq=2)

