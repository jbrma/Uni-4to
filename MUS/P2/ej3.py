import numpy as np
import sounddevice as sd
import kbhit

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

# Ejemplo de uso:
modulator = Modulator(frequency=220, volume_min=0.2, volume_max=0.8)
modulated_signal = modulator.next(chunk_size=1024, sample_rate=44100)
