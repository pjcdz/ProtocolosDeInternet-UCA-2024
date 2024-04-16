# EJ02_02_Socket_Cliente_B.py
import socket
import os
import time

server_dir = ('localhost', 6667)

for i in range(5):  # Crear 5 clientes
    pid = os.fork()

    if pid == 0:  # Estamos en el proceso hijo
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print ('[Proceso hijo %d] conectando a %s puerto %s' % (os.getpid(), server_dir[0], server_dir[1]))
        sock.connect(server_dir)

        for num in range(1, 6):  # Cambiado a 6 para que envíe números del 1 al 5
            # Enviando datos
            mensaje = str(num).encode('utf-8')
            sock.sendall(mensaje)

            # Buscando respuesta
            data = sock.recv(1024)
            print(data.decode('utf-8'))

        # Cerrar la conexión sin esperar confirmación del servidor
        print ('[Proceso hijo %d] cerrando socket' % os.getpid())
        sock.close()
        os._exit(0)  # Terminar el proceso hijo

    else:
        # Esperar a que el proceso hijo termine
        os.wait()
        # Pausa antes de crear el próximo fork
        time.sleep(2)