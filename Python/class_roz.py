import numpy as np

class obrabotka:
    def __init__(self):
        self.amp = []
        self.volt = []
    def update_data(self,amp_new,volt_new):
        self.amp = amp_new
        self.volt = volt_new
    def raschet(self):
        amp = np.array(self.amp)
        volt = np.array(self.volt)
        self.P = np.mean(amp * volt)
        self.P_rms = np.sqrt(np.mean((amp * volt)**2))
        self.P_max = np.max(amp * volt)
        self.P_min = np.min(amp * volt)
        self.coleb = (self.P_max - self.P_min) / self.P * 100
        self.I_rms = np.sqrt(np.mean(amp ** 2))
        self.U_rms = np.sqrt(np.mean(volt ** 2))
        self.S = self.I_rms * self.U_rms
        self.Q = np.sqrt(self.S**2 - self.P**2)
        self.cosfi = self.P / self.S
