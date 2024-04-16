# EJ02_02_Socket_Servidor_Concurrente.py
import socket
import threading

def factorial(n):
    if n == 0:
        return 1
    else:
        return n * factorial(n-1)

def proceso_hijo(conn, addr):
    print('Conexión establecida con', addr)
    while True:
        data = conn.recv(10)
        if not data:
            print('No hay más datos de', addr)
            break
        num = int(data.decode('utf-8'))
        print('Número recibido:', num)
        # Calcular el factorial y enviarlo al cliente
        fact = factorial(num)
        conn.sendall(str(fact).encode('utf-8'))
    conn.close()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_dir = ('localhost', 6667)
print('Socket creado')
sock.bind(server_dir)
print('Enlace del socket completado')
sock.listen(1)
print('Socket en modo escucha')

while True:
    conn, addr = sock.accept()
    threading.Thread(target=proceso_hijo, args=(conn, addr)).start()