import socket
import threading
import os
import time
import math

def proceso_hijo(*args): #*args valores de conexión y dirección de cliente devueltos por sock.accept()
    conn = args[0] #Conexión
    addr = args[1] #Dir Cliente
    try:
        print('Conexión establecida con {}.'.format(addr))
        conn.send("Servidor: Conexión establecida con el cliente".encode('UTF-8'))

        while True:
            data = conn.recv(10)
            if data:
                num = int(data.decode('utf-8'))
                print ('Número recibido: {}'.format(num))

                # Calcular el factorial
                start_time = time.time()
                factorial = math.factorial(num)
                end_time = time.time()

                # Obtener el tiempo de ejecución
                execution_time = end_time - start_time

                # Obtener los PID del proceso hijo y padre
                child_pid = os.getpid()
                parent_pid = os.getppid()

                # Enviar la respuesta al cliente
                response = "\nFactorial de {}: {}\nTiempo de ejecución: {} segundos\nPID del proceso hijo: {}\nPID del proceso padre: {}\n".format(num, factorial, execution_time, child_pid, parent_pid)
                conn.sendall(response.encode('utf-8'))
            else:
                print ('No hay más datos de {}'.format(addr))
                break
    finally:
        conn.close()

#servidor concurrente
host = "127.0.0.1"
port = 6667

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print ("Socket creado")
sock.bind((host, port))
print ("Enlace del socket completado")
sock.listen(1)
print ("Socket en modo escucha")

while 1:
    conn, addr = sock.accept()
    threading.Thread(target=proceso_hijo, args=(conn, addr)).start()