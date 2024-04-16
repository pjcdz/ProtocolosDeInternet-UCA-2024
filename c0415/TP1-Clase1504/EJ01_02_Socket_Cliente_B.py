import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_dir = ('localhost', 6667)
print ('conectando a %s puerto %s' % server_dir)
sock.connect(server_dir)

for num in range(1, 11):
    # Enviando datos
    mensaje = str(num).encode('utf-8')
    # print('enviando "%s"' % mensaje)  # Comentado aquí
    sock.sendall(mensaje)

    # Buscando respuesta
    data = sock.recv(1024)
    # print('recibiendo "%s"' % data.decode('utf-8'))  # Comentado aquí
    print(data.decode('utf-8'))

print ('cerrando socket')
sock.close()