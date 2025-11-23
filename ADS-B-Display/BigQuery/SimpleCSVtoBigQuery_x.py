import winsound
import sys
import os

if len(sys.argv) == 3:
    argument1 = sys.argv[1]
    argument2 = sys.argv[2]
    print(f"The first argument is: {argument1}")
    print(f"The second argument is: {argument2}")
else:
    os._exit(0)
if os.path.exists(argument1+argument2):
    os.remove(argument1+argument2)
frequency = 2500  # Set Frequency (Hz)
duration = 1000 # Set Duration (ms)
winsound.Beep(frequency, duration)