class Test:
    tState = 0

    @thread
    def t1(self):
        if(self.tState == 0):
            self.tState = 1
            cs = True
            del cs
        
    @thread
    def t2(self):
        if(self.tState == 1):
            self.tState = 0
            cs = True
            del cs

    @marker
    def mark_t1(self, state):
        if localvar(state, 't1', 'cs'): return 'blue'
        
    @marker
    def mark_t2(self, state):
        if localvar(state, 't2', 'cs'): return 'blue'
        
    @marker
    def mark_both(self,state):
        if localvar(state, 't1', 'cs') and localvar(state, 't2', 'cs'):
            return 'red'