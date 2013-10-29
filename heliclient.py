import serial
import struct

class HeliClient(object):
    def __init__(self, port, baudrate):
        self.arduino = serial.Serial(port, baudrate=baudrate)
    
    def _printCommand(self):
        print(bin(int(self.arduino.readline().strip())))

    def setThrottle(self, throttle):
        """Sets the throttle of the helicopter to a value between 0 and 127."""
        if throttle > 127:
            throttle = 127
        elif throttle < 0:
            throttle = 0

        throttle_packed = struct.pack('>h', throttle)
        self.arduino.write("\xff\x01\x00" + throttle_packed)
        self._printCommand()
    
    def setPitch(self, pitch):
        """Sets the pitch of the helicopter. Current options are only 1 for forward,
           and 2 for backward."""
        if pitch > 127:
            pitch = 127
        elif pitch < 0:
            pitch = 0
        
        pitch_packed = struct.pack('>h', pitch)
        self.arduino.write("\xff\x03\x00" + pitch_packed)
        self._printCommand()
    
    def setYaw(self, yaw):
        """Sets yaw between 0 and 127. Currently not fully implemented on the Arduino side."""
        self.arduino.write("\xff\x04\x00\x00\x00")
        self._printCommand()
