import socket
from objects import Ad_Socket_Object, Ad_String_Object

def create_server(socket_obj):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('127.0.0.1', 5003))
    sock.listen()
    socket_obj.sock = sock

def create_client(socket_obj):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 5003))
    socket_obj.sock = sock

def accept_socket(socket_obj):
    conn, addr = socket_obj.sock.accept()
    client_sock = Ad_Socket_Object(name="client", host="127.0.0.1", port=5003, is_active=True,
            is_forever=False, is_client=True, is_server=False, sock=None,
            conn=conn, addr=addr)
    return client_sock

def send_socket(socket_obj, message):
    escaped_text = cleanup_unescaped_characters(message.value)
    if socket_obj.conn:
        socket_obj.conn.sendall(escaped_text.encode())

def read_socket(socket_obj):
    message = ""
    while True:
        data = socket_obj.sock.recv(1024)
        if data:
            message += data.decode()
        else:
            break
    result = Ad_String_Object(value=message)
    return result

def close_socket(socket_obj):
    if socket_obj and socket_obj.conn:
        socket_obj.conn.close()

def cleanup_unescaped_characters(raw_text):
    # TODO: fix this
    return raw_text
