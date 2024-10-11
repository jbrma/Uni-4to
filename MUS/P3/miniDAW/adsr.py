
import numpy as np   
import matplotlib.pyplot as plt
from consts import *
from env import *

class ADSR:
    def __init__(self,att,dec,sus,rel):
        self.act = Env([(0,0),(att,1),(att+dec,sus)],xAxis='time')
        self.sus = sus
        self.rel = Env([(0,sus),(rel,0)],xAxis='time')
        self.state = 'off' # act, rel

    def next(self):
        if self.state=='off': 
            return 0
        elif self.state=='act': 
            out = self.act.next()            
            return out
        elif self.state=='rel': 
            out = self.rel.next()   
            # cuando se acaba el release pasa a estado off           
            if out.shape==(): 
                self.state = 'off'
            return out

    def start(self):
        self.state='act'    
        self.act.reset()

    def release(self):
        self.state='rel'    
