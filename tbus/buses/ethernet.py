import socket


class EthernetBus:
    stack = None
    conn = None

    def __init__(self, stack, ip='192.168.1.219 ', port=5000):
        self.stack = stack
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
        self.conn.connect((ip, port))
        
        #self.conn.setblocking(0)

    def send(self, bytes_):
        return self.conn.sendall(bytes_)

    def receive(self, N_bytes):
        toread = N_bytes
        buf = bytearray(toread)
        view = memoryview(buf)

        while toread:
            nbytes = self.conn.recv_into(view, toread)

            view = view[nbytes:] # slicing views is cheap
            toread -= nbytes

        return buf

    def close(self):
        self.conn.close()