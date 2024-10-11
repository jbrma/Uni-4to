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
        data = self.volume*np.sin(2*np.pi*(np.arange(self.current_frame, self.current_frame+chunk_size))*self.frequency/sample_rate)
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

class Modulator(Osc):
    def __init__(self, frequency=440, volume_min=0, volume_max=1):
        super().__init__(frequency)
        self.volume_min = volume_min
        self.volume_max = volume_max

    def next(self, chunk_size=1024, sample_rate=44100):
        modulation_signal = np.linspace(self.volume_min, self.volume_max, chunk_size)
        modulated_data = modulation_signal * super().next(chunk_size, sample_rate)
        return np.float32(modulated_data)


modulator = Modulator(frequency=220, volume_min=0.2, volume_max=0.8)
modulated_signal = modulator.next(chunk_size=1024, sample_rate=44100)

def panner(audio_signal, balance):
    # Verificar si la señal de entrada es estéreo
    if audio_signal.shape[1] != 2:
        raise ValueError("La señal de entrada debe ser estéreo (tener 2 canales)")

    # Asegurarse de que el balance esté en el rango [-1, 1]
    balance = np.clip(balance, -1, 1)

    # Calcular los factores de balance para cada canal
    left_factor = np.sqrt(1 - (balance / 2 + 0.5))
    right_factor = np.sqrt(balance / 2 + 0.5)

    # Aplicar los factores de balance a cada canal
    left_channel = audio_signal[:, 0] * left_factor
    right_channel = audio_signal[:, 1] * right_factor

    # Combinar los canales balanceados en una matriz estéreo
    balanced_signal = np.column_stack((left_channel, right_channel))

    return balanced_signal

"""
# Ejemplo de uso:
# Crear una señal estéreo de ejemplo (muestras aleatorias)
stereo_signal = np.random.randn(44100, 2)
samples = Osc.next()

# Aplicar efecto de balance
balanced_signal = panner(samples, balance=0.5)
"""
