#%% Ejercicio 4
last = 0

def oscDur(freq,dur,vol):
    SRATE1 = 440
    global last # para modificar la var last global (y no una local)
    # el arange se genera desde last en adelante:
    # arange(last, last+CHUNK) -> [last,last+1,last+2...
    # y sumamos last
    data = vol*np.sin(2*np.pi*(np.arange(last,last+dur))*freq/SRATE1)
    last += dur # actualizamos ultimo generado
    return np.float32(data)

def chirp(frecIni,frecFin,dur):
    dif = frecFin-frecIni+1
    tiempOsc= dur/dif
    total= osc(frecIni,tiempOsc,0.5)
    for i in range(frecIni+1, frecFin+1):
        total = np.append(total, osc(i,tiempOsc,0.5))
       
    return total

vaina  = chirp(10,500,2)
plt.plot( vaina[30000:50000])