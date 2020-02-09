from math import log2, log
import sys

def levels(log_q, del_r, t, logbase=2):
  #num = log(float(q)/4, logbase) + log(t, logbase) - log(del_r + 1.25, logbase)
  num = log_q - log(4, logbase) + log(t, logbase) - log(del_r + 1.25, logbase)
  den = log(del_r, logbase) + log(del_r + 1.25, logbase) + log(t, logbase)
  return num/den

def main():
  t_in = 1024
  #First arg is log2(q), second is n AKA del_r
  print(str(levels(float(sys.argv[1]), float(sys.argv[2]), t_in)))

if __name__ == "__main__":
  main()
